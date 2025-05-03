// Author: Kevin Rodriguez
// Course: CS2
// Semester: Spring 2025

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.*;

public class CampusNetworkPlanner 
{
    private List<Edge> edges;
    private int numConnections;
    private String fileName;

    public CampusNetworkPlanner(int numConnections, String fileName) 
    {
        this.numConnections = numConnections;
        this.fileName = fileName;
        this.edges = new ArrayList<>(numConnections);
        readConnections();
    }

    private void readConnections() 
    {
        try (Scanner scanner = new Scanner(new File(fileName))) 
        {
            while (scanner.hasNext()) 
            {
                String campusA = scanner.next();
                String campusB = scanner.next();
                int cost = scanner.nextInt();
                edges.add(new Edge(campusA, campusB, cost));
            }
        } catch (FileNotFoundException e) 
        {
            System.err.println("Input file not found: " + fileName);
            System.exit(1);
        }
    }

    public String buildNetwork() 
    {
        // Gather all unique campuses
        Set<String> campusSet = new HashSet<>();
        for (Edge edge : edges) 
        {
            campusSet.add(edge.campusA);
            campusSet.add(edge.campusB);
        }
        List<String> campusList = new ArrayList<>(campusSet);
        Collections.sort(campusList);

        // Map names to indices for union-find
        Map<String, Integer> campusIndex = new HashMap<>();
        for (int i = 0; i < campusList.size(); i++) 
        {
            campusIndex.put(campusList.get(i), i);
        }

        // Sort edges by cost (Kruskal)
        Collections.sort(edges);
        UnionFind uf = new UnionFind(campusList.size());
        List<Edge> mst = new ArrayList<>();
        int totalCost = 0;
        for (Edge edge : edges) 
        {
            int u = campusIndex.get(edge.campusA);
            int v = campusIndex.get(edge.campusB);
            if (uf.union(u, v)) 
            {
                mst.add(edge);
                totalCost += edge.cost;
                if (mst.size() == campusList.size() - 1) break;
            }
        }

        // Normalize campus order (lexicographically) for output
        List<Edge> outputEdges = new ArrayList<>();
        for (Edge edge : mst) 
        {
            String a = edge.campusA;
            String b = edge.campusB;
            if (a.compareTo(b) <= 0) 
            {
                outputEdges.add(new Edge(a, b, edge.cost));
            } else {
                outputEdges.add(new Edge(b, a, edge.cost));
            }
        }
        Collections.sort(outputEdges, (e1, e2) -> {
            int cmp = e1.campusA.compareTo(e2.campusA);
            return (cmp != 0) ? cmp : e1.campusB.compareTo(e2.campusB);
        });

        // Build formatted output (tab-separated)
        StringBuilder sb = new StringBuilder();
        for (Edge edge : outputEdges) 
        {
            sb.append(edge.campusA)
              .append("---")
              .append(edge.campusB)
              .append("\t$")
              .append(edge.cost)
              .append("\n");
        }
        sb.append("\n");
        sb.append("Total Cost: $").append(totalCost);
        return sb.toString();
    }

    private static class Edge implements Comparable<Edge> 
    {
        String campusA;
        String campusB;
        int cost;

        Edge(String campusA, String campusB, int cost) 
        {
            this.campusA = campusA;
            this.campusB = campusB;
            this.cost = cost;
        }

        @Override
        public int compareTo(Edge other) 
        {
            return Integer.compare(this.cost, other.cost);
        }
    }

    private static class UnionFind 
    {
        private int[] parent;
        private int[] rank;

        UnionFind(int n) 
        {
            parent = new int[n];
            rank = new int[n];
            for (int i = 0; i < n; i++) 
            {
                parent[i] = i;
                rank[i] = 0;
            }
        }

        int find(int x) 
        {
            if (parent[x] != x) 
            {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        boolean union(int x, int y) 
        {
            int rootX = find(x);
            int rootY = find(y);
            if (rootX == rootY) return false;
            if (rank[rootX] < rank[rootY]) 
            {
                parent[rootX] = rootY;
            } else if (rank[rootX] > rank[rootY]) 
            {
                parent[rootY] = rootX;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
            return true;
        }
    }

    private static int countFileLines(String fileName) throws IOException 
    {
        int count = 0;
        try (Scanner scanner = new Scanner(new File(fileName))) 
        {
            while (scanner.hasNextLine()) 
            {
                if (!scanner.nextLine().trim().isEmpty()) count++;
            }
        }
        return count;
    }

    public static void main(String[] args) 
    {
        Scanner console = new Scanner(System.in);
        String fileName = console.nextLine().trim();
        int numConnections;
        try 
            {
            numConnections = countFileLines(fileName);
        } catch (IOException e) 
            {
            System.err.println("Error reading file: " + e.getMessage());
            return;
        }
        CampusNetworkPlanner planner = new CampusNetworkPlanner(numConnections, fileName);
        System.out.println(planner.buildNetwork());
    }
}
