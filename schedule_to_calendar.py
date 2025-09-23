import re
import pytz
import subprocess
import hashlib
import json
from datetime import datetime, timedelta

from bs4 import BeautifulSoup
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service as ChromeService
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager

# ========= CONFIG =========
LOGIN_URL = "https://www.thinkrhino.com/employee/florida/Index.aspx?cookieCheck=true"
SCHEDULE_PAGE_URL = "https://www.thinkrhino.com/employee/florida/Index.aspx?cookieCheck=true"

USERNAME = "YOUR_EMAIL_HERE"        # <- put your Rhino email
PASSWORD = "YOUR_PASSWORD_HERE"     # <- put your Rhino password

USERNAME_SELECTOR = (By.ID, "emailaddress")
PASSWORD_SELECTOR = (By.ID, "mypassword")
LOGIN_BUTTON_SEL  = (By.ID, "btnNewLogin")

VIEW_SCHEDULE_BUTTON = (By.ID, "btnSchedule")

TABLE_ROW_SELECTOR = "table tr"

LOCAL_TZ = pytz.timezone("America/New_York")
DEFAULT_SHIFT_HOURS = 4
CALENDAR_NAME = "Work"              # <- change to the exact Apple Calendar name you want
# ==========================


def login_and_get_html():
    options = webdriver.ChromeOptions()
    options.add_argument("--window-size=1280,1000")
    driver = webdriver.Chrome(service=ChromeService(ChromeDriverManager().install()),
                              options=options)

    driver.get(LOGIN_URL)

    WebDriverWait(driver, 20).until(EC.presence_of_element_located(USERNAME_SELECTOR))
    driver.find_element(*USERNAME_SELECTOR).send_keys(USERNAME)
    driver.find_element(*PASSWORD_SELECTOR).send_keys(PASSWORD)
    driver.find_element(*LOGIN_BUTTON_SEL).click()

    # Dismiss post-login popup if it appears
    try:
        ok_btn = WebDriverWait(driver, 5).until(
            EC.element_to_be_clickable((By.XPATH, "//div[@id='wotc_more_info_msg']//button[normalize-space()='OK']"))
        )
        driver.execute_script("arguments[0].click();", ok_btn)
        print("✔️ Dismissed post-login popup.")
    except:
        pass

    # Click “View My Schedule”
    btn = WebDriverWait(driver, 20).until(
        EC.presence_of_element_located(VIEW_SCHEDULE_BUTTON)
    )
    driver.execute_script("arguments[0].click();", btn)

    # Wait until schedule table rows load (>1 row)
    WebDriverWait(driver, 30).until(
        lambda d: len(d.find_elements(By.CSS_SELECTOR, TABLE_ROW_SELECTOR)) > 1
    )

    html = driver.page_source
    driver.quit()
    return html


def parse_shifts_from_html(html):
    soup = BeautifulSoup(html, "lxml")
    rows = soup.select(TABLE_ROW_SELECTOR)

    date_pat = re.compile(r"\b\d{2}/\d{2}/\d{4}\b")
    time_pat = re.compile(r"\b\d{1,2}:\d{2}\b")
    timerange_pat = re.compile(r"(\d{1,2}:\d{2})\s*[-–]\s*(\d{1,2}:\d{2})")

    shifts = []

    for tr in rows:
        # Skip cancelled (crossed-out) shifts
        if any("line-through" in (td.get("style") or "").lower() for td in tr.find_all("td")):
            continue

        cells = [td.get_text(" ", strip=True) for td in tr.find_all(["td", "th"])]
        if len(cells) < 3:
            continue

        # Date
        date_str = next((c for c in cells if date_pat.search(c)), None)
        if not date_str:
            continue
        date_val = date_pat.search(date_str).group(0)  # mm/dd/yyyy

        # Time or time range
        joined = "  ".join(cells)
        m = timerange_pat.search(joined)
        if m:
            t1, t2 = m.group(1), m.group(2)
            start_dt_local = _localize(date_val, t1)
            end_dt_local = _localize(date_val, t2)
            if end_dt_local <= start_dt_local:
                end_dt_local += timedelta(days=1)
        else:
            time_cell = next((c for c in cells if time_pat.search(c)), None)
            if not time_cell:
                continue
            t = time_pat.search(time_cell).group(0)
            start_dt_local = _localize(date_val, t)
            end_dt_local = start_dt_local + timedelta(hours=DEFAULT_SHIFT_HOURS)

        # Event details (heuristics from your columns)
        show     = cells[3] if len(cells) > 4 else ""
        venue    = cells[4] if len(cells) > 5 else ""
        location = cells[5] if len(cells) > 6 else ""
        client   = cells[6] if len(cells) > 7 else ""
        position = cells[8] if len(cells) > 9 else ""
        status   = cells[10] if len(cells) > 10 else ""

        summary = f"{position or 'Shift'} — {show or venue or client}".strip(" —")
        desc_parts = []
        if show:   desc_parts.append(f"Show: {show}")
        if venue:  desc_parts.append(f"Venue: {venue}")
        if client: desc_parts.append(f"Client: {client}")
        if status: desc_parts.append(f"Status: {status}")
        description = "\n".join(desc_parts) if desc_parts else "Rhino shift"

        # Stable UID for dedupe
        uid_input = f"{summary}-{start_dt_local.isoformat()}"
        uid = hashlib.md5(uid_input.encode()).hexdigest()
        description = f"{description}\n\nShiftUID:{uid}"

        shifts.append({
            "start": start_dt_local,     # timezone-aware
            "end": end_dt_local,         # timezone-aware
            "summary": summary or "Rhino Shift",
            "location": f"{venue} {location}".strip(),
            "description": description,
            "uid": uid
        })

    return shifts


def _localize(date_mmddyyyy, time_hhmm):
    # date: mm/dd/yyyy, time: HH:MM (24h)
    dt_naive = datetime.strptime(f"{date_mmddyyyy} {time_hhmm}", "%m/%d/%Y %H:%M")
    return LOCAL_TZ.localize(dt_naive)


def event_exists_in_calendar(uid):
    # AppleScript to search description for ShiftUID
    script = f'''
    tell application "Calendar"
      tell calendar "{CALENDAR_NAME}"
        set theEvents to every event whose description contains "ShiftUID:{uid}"
        if (count of theEvents) > 0 then
          return "YES"
        else
          return "NO"
        end if
      end tell
    end tell
    '''
    result = subprocess.run(["osascript", "-e", script], capture_output=True, text=True)
    return result.stdout.strip() == "YES"


def add_event_to_calendar(event):
    # Dedupe check
    if event_exists_in_calendar(event["uid"]):
        print(f"⏩ Skipping duplicate: {event['summary']} ({event['start']})")
        return

    # Use ISO 8601 w/ timezone (JXA parses this correctly)
    start_iso = event["start"].isoformat()  # e.g., 2025-09-23T08:00:00-04:00
    end_iso   = event["end"].isoformat()

    # Safely JSON-encode strings so quotes/newlines don’t break the JXA code
    cal_name_js = json.dumps(CALENDAR_NAME)
    summary_js  = json.dumps(event["summary"])
    location_js = json.dumps(event["location"])
    notes_js    = json.dumps(event["description"])
    start_js    = json.dumps(start_iso)
    end_js      = json.dumps(end_iso)

    jxa = f'''
    var app = Application("Calendar");
    var cals = app.calendars.whose({{name: {cal_name_js}}})();
    if (cals.length === 0) {{
      throw new Error("Calendar named {CALENDAR_NAME} not found");
    }}
    var cal = cals[0];
    var ev = app.Event({{
      summary: {summary_js},
      location: {location_js},
      startDate: new Date({start_js}),
      endDate: new Date({end_js}),
      description: {notes_js}
    }});
    cal.events.push(ev);
    '''
    subprocess.run(["osascript", "-l", "JavaScript", "-e", jxa])
    print(f"✅ Added: {event['summary']} ({event['start'].strftime('%Y-%m-%d %H:%M')})")


def main():
    html = login_and_get_html()
    if not html:
        return

    shifts = parse_shifts_from_html(html)
    if not shifts:
        print("⚠️ No shifts parsed.")
        return

    for ev in shifts:
        add_event_to_calendar(ev)

    print(f"🎉 Done. Processed {len(shifts)} shifts into '{CALENDAR_NAME}'.")
    

if __name__ == "__main__":
    main()
