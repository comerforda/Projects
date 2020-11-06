// 5611 Animation & Planning project 3 - path planning using PRMs & search

// Setup vars
int drawing = 1;
int placing = 1;
int startPlaced = 0;
int goalPlaced = 0;
int prmDraw = 0;
// Pixel vars
int[] whitePix = new int[900*800];
int whiteCount = 0;
// Node vars
int maxNodes = 750;
int numNodes = 750;
Vec2[] nodePos = new Vec2[maxNodes];
Vec2 startNode, goalNode;
// UCS vars
ArrayList<Integer>[] neighbors = new ArrayList[maxNodes];
Boolean[] visited = new Boolean[maxNodes];
int[] parent = new int[maxNodes];
// path vars
Vec2[] curPath;
Vec2 agentPos;
int pathLen;
int pathIdx;

PFont font;

void setup() {
  size(900, 800);
  background(10);
  textSize(20);
  //font = loadFont("LetterGothicStd-32.vlw");
}

void draw() {
  //background(10);
  float dt = (1.0/frameRate);
  if (placing == 0) {
    update(dt);
  }
  // Draw the white walls
  stroke(255);
  strokeWeight(5);
  if (mousePressed == true) {
    line(mouseX, mouseY, pmouseX, pmouseY);
  }
  
  
  // Draw the PRM
  if (prmDraw == 1) {
    noStroke();
    /*
    if (drawing == 0) {
      fill(0,220,0);
      for (int i = 0; i < numNodes; i++) {
        circle(nodePos[i].x, nodePos[i].y, 2);
      }
    }
    */
     //Draw graph
    stroke(0, 200, 50);
    strokeWeight(.5);
    for (int i = 0; i < numNodes; i++){
      for (int j : neighbors[i]){
        line(nodePos[i].x,nodePos[i].y,nodePos[j].x,nodePos[j].y);
      }
    }
  }
  
  // Draw start and Goal
  if (startPlaced == 1){
    fill(220,0,0);
    stroke(220,0,0);
    circle(startNode.x, startNode.y, 3);
  }
  if (goalPlaced == 1) {
    fill(20,150,250);
    stroke(20,150,250);
    circle(goalNode.x, goalNode.y, 3);
    // Draw the agent
    fill(0,220,0);
    stroke(0,220,0);
    strokeWeight(1.5);
    circle(agentPos.x, agentPos.y, 1.5);
  }
}

void update(float dt) {
  Vec2 direction = curPath[pathIdx].minus(agentPos);
  // This skips nodes if it can reach the next one
  // in the path without colliding
  for (int i = pathIdx; i < pathLen; i++) {
    Vec2 dir = curPath[i].minus(agentPos);
    float len = dir.length();
    dir.normalize();
    if(!hitWall(agentPos, dir, len)) {
      direction = curPath[i].minus(agentPos);
      pathIdx = i;
    }
  }
  
  if (direction.length() > 1.0) {
    direction.normalize();
    direction.mul(50);
  }
  agentPos.add(direction.times(dt));
}

// Sets a new array of only white pixels
void setWhitePix() {
  for (int i = 0; i < width * height; i++) {
    if (pixels[i] == color(255)){
      whitePix[whiteCount] = i;  //set the pixel idx that's white into whitePix
      whiteCount++;
    }
  }
}

// Returns the x value of a pixel location
int getX (int i) {
  int y = i / width;
  int x = i - (y * width);
  return x;
}

// Returns the y value of a pixel location
int getY (int i) {
  return i/width;
}

void placeStartGoal() {
  if (startPlaced == 1 && goalPlaced == 0) {
    goalNode = new Vec2(mouseX, mouseY);
    goalPlaced = 1;
  }
  if (startPlaced == 0) {
    startNode = new Vec2(mouseX, mouseY);
    agentPos = new Vec2(mouseX, mouseY);
    startPlaced = 1;
  }
}

void initNodePath() {
  pathLen = 0;
  pathIdx = 0;
  // If we've already placed our nodes before, set
  // the current position as the start node
  if (placing == 0) {
    startNode = new Vec2(agentPos.x, agentPos.y);
    startPlaced = 0;
  }
  generateRandomNodes();
  connectNeighbors();
  ArrayList<Integer> path = UCS();
  // check if there is no path
  if (path.get(0) == -1){
    text("Game Over", 375, 100);
    placing = 1;
    return;
  }
  placing = 0;
  pathLen = path.size();
  curPath = new Vec2[pathLen];
  for (int i = 0; i < path.size(); i++) {
    curPath[i] = new Vec2(nodePos[path.get(i)].x, nodePos[path.get(i)].y);
  }
}

void keyPressed()
{
  // reset your drawing
  if (key == 'r') {
    background(10);
    drawing = 1;
    placing = 1;
    startPlaced = 0;
    goalPlaced = 0;
  }
  if (drawing == 1 && key == ' '){
    drawing = 0;
    loadPixels();
    setWhitePix();
  }
  
  if (goalPlaced == 1 && key == ' ') {
    initNodePath();
  }
  
  if (key == 'p' && drawing == 0 && placing == 0) {
    if (prmDraw == 0)
      prmDraw = 1;
    else
      prmDraw = 0;
  }
}

void mousePressed() {
  if (drawing == 0 && placing == 1) {
    placeStartGoal();
  }
}

void mouseReleased() {
  if (placing == 0 && drawing == 0) {
    //drawing = 1;
    loadPixels();
    setWhitePix();
    initNodePath();
  }
}
