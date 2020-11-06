ArrayList<Integer> UCS() {
  ArrayList<Integer> fringe = new ArrayList();  // new empty fringe
  float costs[] = new float[maxNodes];
  ArrayList<Integer> path = new ArrayList();
  for (int i = 0; i < numNodes; i++) {
    visited[i] = false;
    parent[i] = -1;
  }
  fringe.add(0);
  costs[0] = 0;
  float min = 0;
  int min_fringe = 0;
  
  while (fringe.size() > 0) {
    int current = fringe.get(min_fringe);
    fringe.remove(min_fringe);
    
    // mark as visited
    if (!visited[current])
      visited[current] = true;
    // check if we're at goal
    if (current == numNodes)
      break;
    
    // Add neighbors to the fringe
    for (int i = 0; i < neighbors[current].size(); i++) {
      int neighbor = neighbors[current].get(i);
      if (!visited[neighbor]) {
        visited[neighbor] = true;
        fringe.add(neighbor);
        parent[neighbor] = current;
        // add the cost from the parent to the neighbor
        costs[neighbor] = costs[current] + nodePos[current].distanceTo(nodePos[neighbor]);
      }
    }
    
    // find the min-cost node in the fringe
    min = 9999.9;
    for (int i = 0; i < fringe.size(); i++) {
      if (costs[fringe.get(i)] < min) {
        min = costs[fringe.get(i)];
        min_fringe = i;
      }
    }
  }
  
  if (parent[numNodes-1] == -1){
    println("No Path");
    path.add(0,-1);
    return path;
  }
  
  int prevNode = parent[numNodes-1];
  path.add(0,numNodes-1);
  //print(goalID, " ");
  while (prevNode >= 0){
    print(prevNode," ");
    path.add(0,prevNode);
    prevNode = parent[prevNode];
  }
  
  return path;
}
