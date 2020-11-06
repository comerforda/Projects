void generateRandomNodes() {
  // Add our Start and Goal nodes
  nodePos[0] = startNode;
  nodePos[maxNodes-1] = goalNode;
  // Add random nodes
  for (int i = 1; i < maxNodes-1; i++) {
    Vec2 randPos;
    // Get some random while pixel
    int whiteIdx = int(random(whiteCount));
    int x = getX(whitePix[whiteIdx]);  //x coord of white pix
    int y = getY(whitePix[whiteIdx]);  //y coord of white pix

    // Set the point to a position within 30 of the random white pixel
    if (i % 2 == 0)
      randPos = new Vec2(random(x - 40, x - 5), random(y - 40, y - 5));
    else
      randPos = new Vec2(random(x + 5, x + 40), random(y + 5, y + 40));
    moveNode(randPos);  //move the node if it's close to the line
    boolean inWall = pointInWall(randPos);
    while (inWall) {     
      randPos = new Vec2(random(x + 10, x + 40), random(y + 10, y + 40));
      moveNode(randPos);
      inWall = pointInWall(randPos);
    }
    nodePos[i] = randPos;
  }
}

// If it's too close to the wall - move it out.
void moveNode(Vec2 point) {
  int x;
  int y;
  for (int i = 0; i < whiteCount; i++) {
    x = getX(whitePix[i]);
    y = getY(whitePix[i]);
    Vec2 linePos = new Vec2(x,y);
    Vec2 dist = point.minus(linePos);
    if (dist.length() < 5){
      Vec2 norm = dist.normalized();
      point.add(norm.times(5));
    }
  }
}

void connectNeighbors() {
  for (int i = 0; i < numNodes; i++) {
    neighbors[i] = new ArrayList<Integer>();
    for (int j = 0; j < numNodes; j++) {
      if (i == j) continue; // Don't connect yourself
      Vec2 dir = nodePos[j].minus(nodePos[i]).normalized();
      float dist = nodePos[i].distanceTo(nodePos[j]);
      
      boolean hitsWall = hitWall(nodePos[i], dir, dist);
      if (!hitsWall && dist < 200)
        neighbors[i].add(j);
    }
  }
}

// This detects if two nodes have a white wall between them
// or if the line between the nodes is within the bounding size 
// (3 pixels) of a white line.
boolean hitWall(Vec2 nodePos, Vec2 dir, float dist) {
  boolean val = true;
  Vec2 position = new Vec2(nodePos.x, nodePos.y);
  Vec2 boundX = new Vec2(nodePos.x + 3, nodePos.y);
  Vec2 boundXneg = new Vec2(nodePos.x - 3, nodePos.y);
  Vec2 boundY = new Vec2(nodePos.x, nodePos.y + 3);
  Vec2 boundYneg = new Vec2(nodePos.x, nodePos.y-3);
  for (int i = 0; i < whiteCount; i++) {
    if (pointInWall(nodePos) || pointInWall(boundX) ||
        pointInWall(boundXneg) || pointInWall(boundY) ||
        pointInWall(boundYneg)) {
      val = true;
      break;
    }
    else if (nodePos.minus(position).length() >= dist){
      val = false;
      break;
    }
    else {
      nodePos = nodePos.plus(dir);
      boundX = new Vec2(nodePos.x + 3, nodePos.y);
      boundXneg = new Vec2(nodePos.x - 3, nodePos.y);
      boundY = new Vec2(nodePos.x, nodePos.y + 3);
      boundYneg = new Vec2(nodePos.x, nodePos.y-3);
    }
  }
  return val;
}

// Returns true if the point is in a wall
boolean pointInWall(Vec2 point) {
  int x = int(point.x);
  int y = int(point.y);
  boolean val = false;
  int pix_idx = x + y * width;
  if (pixels[pix_idx] == color(255)) {
    val = true;
  }
  return val;
}

/*
//Potential rectangle objects if move to 3D
void generateObstacles() {
  for (int i = 0; i < maxObstacles; i++) {
    float x = random(width);
    float y = random(height);
    float z = random(-500,500);
    obsPos[i] = new Vec3(x,y,z);
    x = random(50);
    y = random(50, 100);
    z = random(50);
    obsDim[i] = new Vec3(x,y,z);
  }
}
*/
