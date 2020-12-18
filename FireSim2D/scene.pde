// grass reference: https://creativemarket.com/fotonomada/4434606-Portion-of-grass-texture
// sky reference: https://www.shutterstock.com/video/clip-1007053117-beautiful-star-trails-dark-blue-sky-night
// Stick, and trees reference: https://drive.google.com/drive/u/0/folders/1aRQUoDFDjEUdB-7Ol0cz9Bftv-kCkog-

void drawScene(){
  
  //Draw Stick (using Staff model from openGameArt models)
  if (has_stick == 1) {
    pushMatrix();
    translate(camera.position.x+hor, camera.position.y+vert, camera.position.z-18);
    //translate(camera.position.x+2, camera.position.y+2, camera.position.z-18);
    //box(0.2, 0.2, 20);
    rotateX(-PI/2);
    shape(stick, 0,0, 0.75, 25);
    popMatrix();
  }
  else {
    pushMatrix();
    translate(400, height-10, 20);
    //translate(camera.position.x+2, camera.position.y+2, camera.position.z-18);
    //box(0.2, 0.2, 20);
    rotateX(-PI/2);
    shape(stick, 0,0, 5, 60);
    popMatrix();
  }
  
  // Draw mallow
    pushMatrix();
    fill(mallow_color);
    //translate(camera.position.x+hor, camera.position.y+vert, camera.position.z-30);
    translate(mallowPos.x, mallowPos.y, mallowPos.z);
    if (has_mallow == 1)
      box(0.85,0.75, 2.5);
    else
      box(4, 5, 4);
    popMatrix();
  
  // Draw Grass
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      beginShape();
      texture(grass);
      vertex(-5000 + i*1000, height, -5000 + j*1000, 0, 0);
      vertex(-5000 + i*1000 + 1000, height, -5000 + j*1000, grass.width, 0);
      vertex(-5000 + i*1000 + 1000, height, -5000 + j*1000 + 1000, grass.width, grass.height);
      vertex(-5000 + i*1000, height, -5000 + j*1000 + 1000, 0, grass.height);
      endShape();
    }
  }

  // Draw Sky
  beginShape();
  texture(sky);
  vertex(-5000, -5000, -5000, 0, 0);
  vertex(5000, -5000, -5000, sky.width, 0);
  vertex(5000, height, -5000, sky.width, sky.height);
  vertex(-5000, height, -5000, 0, sky.height);
  endShape();
  
  beginShape();
  texture(sky);
  vertex(5000, -5000, -5000, 0, 0);
  vertex(5000, -5000, 5000, sky.width, 0);
  vertex(5000, height, 5000, sky.width, sky.height);
  vertex(5000, height, -5000, 0, sky.height);
  endShape();
  
  beginShape();
  texture(sky);
  vertex(-5000, -5000, 5000, 0, 0);
  vertex(5000, -5000, 5000, sky.width, 0);
  vertex(5000, height, 5000, sky.width, sky.height);
  vertex(-5000, height, 5000, 0, sky.height);
  endShape();
  
  beginShape();
  texture(sky);
  vertex(-5000, -5000, -5000, 0, 0);
  vertex(-5000, -5000, 5000, sky.width, 0);
  vertex(-5000, height, 5000, sky.width, sky.height);
  vertex(-5000, height, -5000, 0, sky.height);
  endShape();
  
  beginShape();
  texture(sky);
  vertex(-5000, -5000, -5000, 0, 0);
  vertex(5000, -5000, -5000, sky.width, 0);
  vertex(5000, -5000, 5000, sky.width, sky.height);
  vertex(-5000, -5000, 5000, 0, sky.height);
  endShape();
  
  // Draw logs
  fill(105, 22, 22);
  pushMatrix();
  translate(450, 795, 0);
  box(5, 5, 50);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(460, 790, 5);
  rotateZ(PI/12);
  rotateY(-PI/12);
  box(50, 5, 5);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, -7);
  rotateZ(-PI/12);
  rotateY(-PI/8);
  box(50, 5, 5);
  popMatrix();
  
  fill(105, 22, 22);
  pushMatrix();
  translate(440, 790, 5);
  rotateZ(-PI/12);
  rotateY(PI/12);
  box(50, 5, 5);
  popMatrix();
  
  // Draw Trees
  // North side
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(-500 + i * 200, height, -1000);
    rotateZ(PI);
    if (i == 3)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
  
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(-400 + i * 200, height, -800);
    rotateZ(PI);
    if (i == 7)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
  // West Side
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(-250 + i%2 * 100, height, -800 + i * 200);
    rotateZ(PI);
    if (i < 3)  rotateY(-PI/4);
    else
    rotateY(PI/2);
    if (i ==7)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
  
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(-350 + i%2 * -100, height, -800 + i * 200);
    rotateZ(PI);
    rotateY(PI/2);
    if (i == 5)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
  
  // East Side
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(950 + i%2 * -100, height, -800 + i * 200);
    rotateZ(PI);
    if (i < 4)  rotateY(PI/4);
    else
      rotateY(-PI/2);
    if (i == 5)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
  
  for (int i = 0; i < 10; i++) {
    pushMatrix();
    translate(1050 + i%2 * 100, height, -800 + i * 200);
    rotateZ(PI);
    rotateY(-PI/2);
    if (i == 7)
      shape(birch, 0, 0, 200, 450);
    else
      shape(tree, 0, 0, 200, 300);
    popMatrix();
  }
}
