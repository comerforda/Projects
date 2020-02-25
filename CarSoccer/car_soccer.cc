/** CSci-4611 Assignment 2:  Car Soccer
 */

#include "car_soccer.h"
#include "config.h"
#include <time.h>


CarSoccer::CarSoccer() : GraphicsApp(1024, 768, "Car Soccer") {
    // Define a search path for finding data files (images and shaders)
    searchPath_.push_back(".");
    searchPath_.push_back("./data");
    searchPath_.push_back(DATA_DIR_INSTALL);
    searchPath_.push_back(DATA_DIR_BUILD);
}

CarSoccer::~CarSoccer() {
}


Vector2 CarSoccer::joystick_direction() {
    Vector2 dir;
    if (IsKeyDown(GLFW_KEY_LEFT))
        dir[0]--;
    if (IsKeyDown(GLFW_KEY_RIGHT))
        dir[0]++;
    if (IsKeyDown(GLFW_KEY_UP))
        dir[1]++;
    if (IsKeyDown(GLFW_KEY_DOWN))
        dir[1]--;
    return dir;
}


void CarSoccer::OnSpecialKeyDown(int key, int scancode, int modifiers) {
    if (key == GLFW_KEY_SPACE) {
        launch_ball();
    }
}
void CarSoccer::launch_ball() {
    srand(time(0));
    Vector3 velocity(rand() % 5 + (rand() % 40 - 22.5),
        rand() % 5 + (rand() % 20 - 11.5),
        rand() % 5 + (rand() % 40 - 22.5));    //set random velocity in range (-22.5, 22.5)
    ball_.set_velocity(velocity);

}

void CarSoccer::UpdateSimulation(double timeStep) {
    // Here's where you shound do your "simulation", updating the positions of the
    // car and ball as needed and checking for collisions.

    Point3 p_ball = ball_.position();
    Point3 car_pos = car_.position();
    Vector3 car_vel = car_.velocity();
    Vector3 v_ball = ball_.velocity();

    // Car movement - this must be weird somehow as I had to change the car drawing rotation
    // matrix (line 162) as it was rotating opposite direction of my velocity, but it works!
    Vector2 dir = joystick_direction();
    float thrust = car_.speed() + 0.5 * dir[1];
    float drag = 0.01 * car_.speed();               //simulated drag
    float turnRate = 1.5 * dir[0] * timeStep;       //angle adjustment
    float dx = std::sin(car_.angle());
    float dz = std::cos(car_.angle());
    car_.set_velocity(car_.speed() * Vector3(dx, 0, -dz));
    car_.set_speed(thrust - drag);
    car_.set_angle(car_.angle() + turnRate);


    // Wall collisions. For ball can just negate corresponding component (x = -x) as we are parallel to an axis
    //ground collision
    if (p_ball[1] - ball_.radius() < 0) {
        p_ball[1] = 0 + ball_.radius();
        ball_.set_position(p_ball);
        v_ball[1] = -v_ball[1];
        v_ball = .90 * v_ball;      //Decrease speed to simulate friction
        ball_.set_velocity(v_ball);
    }
    //ceiling collision
    if (p_ball[1] - ball_.radius() >= 35) {
        p_ball[1] = 35 - ball_.radius();
        ball_.set_position(p_ball);
        v_ball[1] = -v_ball[1];
        v_ball = .85 * v_ball;      //Decrease speed to simulate friction
        ball_.set_velocity(v_ball);
    }
    //right wall collision
    if (p_ball[0] + ball_.radius() >= 40) {
        p_ball[0] = 40 - ball_.radius();
        ball_.set_position(p_ball);
        v_ball[0] = -v_ball[0];
        v_ball = .85 * v_ball;      //Decrease speed to simulate friction
        ball_.set_velocity(v_ball);
    }

    if (car_pos[0] + car_.collision_radius() > 40) {
        car_pos[0] = 40 - car_.collision_radius();
        car_vel[0] = 0;
        car_.set_position(car_pos);
        car_.set_velocity(car_vel);
    }

    //left wall collision
    if (p_ball[0] - ball_.radius() <= -40) {
        p_ball[0] = -40 + ball_.radius();
        ball_.set_position(p_ball);
        v_ball[0] = -v_ball[0];
        v_ball = .85 * v_ball;      //Decrease speed to simulate friction
        ball_.set_velocity(v_ball);
    }

    if (car_pos[0] - car_.collision_radius() < -40) {
        car_pos[0] = -40 + car_.collision_radius();
        car_.set_position(car_pos);
    }
    //south wall collision
    if (p_ball[2] + ball_.radius() >= 50) {
        if (p_ball[0] <= 10 && p_ball[0] >= -10 && p_ball[1] <= 10) {  //!!!GOOOOOAAAALLLL!!!
            car_.Reset();
            ball_.Reset();
        }
        else {
            p_ball[2] = 50 - ball_.radius();
            ball_.set_position(p_ball);
            v_ball[2] = -v_ball[2];
            v_ball = .85 * v_ball;      //Decrease speed to simulate friction
            ball_.set_velocity(v_ball);
        }
    }

    if (car_pos[2] + car_.collision_radius() > 50) {
        car_pos[2] = 50 - car_.collision_radius();  //simulate bumping
        car_.set_position(car_pos);
    }

    //north wall collision
    if (p_ball[2] - ball_.radius() <= -50) {
        if (p_ball[0] <= 10 && p_ball[0] >= -10 && p_ball[1] <= 10) {  //!!!GOOOOOAAAALLLL!!!
            car_.Reset();
            ball_.Reset();
        }
        else {
            p_ball[2] = -50 + ball_.radius();
            ball_.set_position(p_ball);
            v_ball[2] = -v_ball[2];
            v_ball = .9 * v_ball;      //Decrease speed to simulate friction
            ball_.set_velocity(v_ball);
        }
    }

    if (car_pos[2] - car_.collision_radius() < -50) {
        car_pos[2] = -50 + car_.collision_radius();
        car_.set_position(car_pos);
    }


    Vector3 distance = ball_.position() - car_.position();
    if (distance.Length() <= ball_.radius() + car_.collision_radius()) {
        Vector3 normal = distance.ToUnit();
        
        float depth = ball_.radius() + car_.collision_radius() - distance.Length();   //amt ball has clipped car
        float epsilon = 0.1;                               //A little extra "fudge" factor to prevent sticking
        p_ball = p_ball + (normal * (depth + epsilon));    //moving the ball back along normal to prevent clipping
        
        Vector3 v_rel = v_ball - car_vel; 
        v_rel = v_rel - 2 * v_rel.Dot(normal) * normal;
        
        Vector3 v_final = (v_rel + car_vel);
        v_final[1] = v_final[1] * 0.75;              //scale the y component down so it's not annoying
        ball_.set_velocity(v_final);
        
    }

    //Gravity for ball - only apply gravity if not colliding and above ground.
    else if (p_ball[1] - ball_.radius() > 0) {
        p_ball = ball_.position();
        v_ball[1] = v_ball[1] - timeStep * 20.81;    // v2 = v1 - dt * g  gravitational acceleration eq.
        ball_.set_velocity(v_ball);
    }

    ball_.set_position(ball_.position() + timeStep * ball_.velocity());
    car_.set_position(car_.position() + timeStep * car_.velocity());

}


void CarSoccer::InitOpenGL() {
    // Set up the camera in a good position to see the entire field
    projMatrix_ = Matrix4::Perspective(60, aspect_ratio(), 1, 1000);
    viewMatrix_ = Matrix4::LookAt(Point3(0, 60, 70), Point3(0, 0, 10), Vector3(0, 1, 0));

    // Set a background color for the screen
    glClearColor(0.8, 0.8, 0.8, 1);

    // Load some image files we'll use
    fieldTex_.InitFromFile(Platform::FindFile("pitch.png", searchPath_));
    crowdTex_.InitFromFile(Platform::FindFile("crowd.png", searchPath_));
}


void CarSoccer::DrawUsingOpenGL() {
    // Draw the crowd as a fullscreen background image
    quickShapes_.DrawFullscreenTexture(Color(1, 1, 1), crowdTex_);

    // Draw the field with the field texture on it.
    Color col(16.0 / 255.0, 46.0 / 255.0, 9.0 / 255.0);
    Matrix4 M = Matrix4::Translation(Vector3(0, -0.201, 0)) * Matrix4::Scale(Vector3(50, 1, 60));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, col);
    M = Matrix4::Translation(Vector3(0, -0.2, 0)) * Matrix4::Scale(Vector3(40, 1, 50));
    quickShapes_.DrawSquare(modelMatrix_ * M, viewMatrix_, projMatrix_, Color(1, 1, 1), fieldTex_);

    // Draw the car
    Color carcol(0.8, 0.2, 0.2);
    Matrix4 Mcar =
        Matrix4::Translation(car_.position() - Point3(0, 0, 0)) *
        Matrix4::RotationY(-car_.angle()) *     //Had to change to -car_angle() cause it was rotating opposite my velocity
        Matrix4::Scale(car_.size()) *
        Matrix4::Scale(Vector3(0.5, 0.5, 0.5));
    quickShapes_.DrawCube(modelMatrix_ * Mcar, viewMatrix_, projMatrix_, carcol);


    // Draw the ball
    Color ballcol(1, 1, 1);
    Matrix4 Mball =
        Matrix4::Translation(ball_.position() - Point3(0, 0, 0)) *
        Matrix4::Scale(Vector3(ball_.radius(), ball_.radius(), ball_.radius()));
    quickShapes_.DrawSphere(modelMatrix_ * Mball, viewMatrix_, projMatrix_, ballcol);


    // Draw the ball's shadow -- this is a bit of a hack, scaling Y by zero
    // flattens the sphere into a pancake, which we then draw just a bit
    // above the ground plane.
    Color shadowcol(0.2, 0.4, 0.15);
    Matrix4 Mshadow =
        Matrix4::Translation(Vector3(ball_.position()[0], -0.1, ball_.position()[2])) *
        Matrix4::Scale(Vector3(ball_.radius(), 0, ball_.radius())) *
        Matrix4::RotationX(90);
    quickShapes_.DrawSphere(modelMatrix_ * Mshadow, viewMatrix_, projMatrix_, shadowcol);


    // You should add drawing the goals and the boundary of the playing area
    // using quickShapes_.DrawLines()

    //Draw the playing area boundary
    std::vector<Point3> boundaries;
    boundaries.push_back(Point3(40.0, 0, -50.0));
    boundaries.push_back(Point3(-40.0, 0, -50.0));
    boundaries.push_back(Point3(-40.0, 0, 50.0));
    boundaries.push_back(Point3(40.0, 0, 50.0));
    quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, Color(1, 1, 1), boundaries, QuickShapes::LinesType::LINE_LOOP, 0.35);

    //Draw orange (south) goal
    std::vector<Point3> goal;
    for (int i = 0; i < 11; i++) {
        goal.push_back(Point3(-10, i, 50));
        goal.push_back(Point3(10, i, 50));
    }

    for (int i = 0; i < 11; i++) {
        goal.push_back(Point3(-10 + i * 2, 0, 50));
        goal.push_back(Point3(-10 + i * 2, 10, 50));
    }
    quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, Color(1, .75, 0), goal, QuickShapes::LinesType::LINES, 0.15);

    //Draw blue (north) goal
    std::vector<Point3> goal2;
    for (int i = 0; i < 11; i++) {
        goal2.push_back(Point3(-10, i, -50));
        goal2.push_back(Point3(10, i, -50));
    }

    for (int i = 0; i < 11; i++) {
        goal2.push_back(Point3(-10 + i * 2, 0, -50));
        goal2.push_back(Point3(-10 + i * 2, 10, -50));
    }
    quickShapes_.DrawLines(modelMatrix_, viewMatrix_, projMatrix_, Color(0, .75, 1), goal2, QuickShapes::LinesType::LINES, 0.15);


    // !! DEBUGGING !!

    //quickShapes_.DrawArrow(modelMatrix_, viewMatrix_, projMatrix_, Color(1, 1, 0), ball_.position(), ball_.velocity(), 0.1);  //draw velocity arrow
    //quickShapes_.DrawArrow(modelMatrix_, viewMatrix_, projMatrix_, Color(1, 1, 0), car_.position(), 5.0 * car_.velocity().ToUnit(), 0.1);  //draw velocity arrow

}