/** CSci-4611 Assignment 3:  Earthquake
 */

#include "quake_app.h"
#include "config.h"

#include <iostream>
#include <sstream>

// Number of seconds in 1 year (approx.)
const int PLAYBACK_WINDOW = 12 * 28 * 24 * 60 * 60;

using namespace std;

QuakeApp::QuakeApp() : GraphicsApp(1280,720, "Earthquake"),
    playback_scale_(15000000.0), debug_mode_(false)
{
    // Define a search path for finding data files (images and earthquake db)
    search_path_.push_back(".");
    search_path_.push_back("./data");
    search_path_.push_back(DATA_DIR_INSTALL);
    search_path_.push_back(DATA_DIR_BUILD);
    
    quake_db_ = EarthquakeDatabase(Platform::FindFile("earthquakes.txt", search_path_));
    current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
   
 }


QuakeApp::~QuakeApp() {
}


void QuakeApp::InitNanoGUI() {
    // Setup the GUI window
    nanogui::Window *window = new nanogui::Window(screen(), "Earthquake Controls");
    window->setPosition(Eigen::Vector2i(10, 10));
    window->setSize(Eigen::Vector2i(400,200));
    window->setLayout(new nanogui::GroupLayout());
    
    date_label_ = new nanogui::Label(window, "Current Date: MM/DD/YYYY", "sans-bold");
    
    globe_btn_ = new nanogui::Button(window, "Globe");
    globe_btn_->setCallback(std::bind(&QuakeApp::OnGlobeBtnPressed, this));
    globe_btn_->setTooltip("Toggle between map and globe.");
    
    new nanogui::Label(window, "Playback Speed", "sans-bold");
    
    nanogui::Widget *panel = new nanogui::Widget(window);
    panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                            nanogui::Alignment::Middle, 0, 20));
    
    nanogui::Slider *slider = new nanogui::Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(120);
    
    speed_box_ = new nanogui::TextBox(panel);
    speed_box_->setFixedSize(Eigen::Vector2i(60, 25));
    speed_box_->setValue("50");
    speed_box_->setUnits("%");
    slider->setCallback(std::bind(&QuakeApp::OnSliderUpdate, this, std::placeholders::_1));
    speed_box_->setFixedSize(Eigen::Vector2i(60,25));
    speed_box_->setFontSize(20);
    speed_box_->setAlignment(nanogui::TextBox::Alignment::Right);
    
    nanogui::Button* debug_btn = new nanogui::Button(window, "Toggle Debug Mode");
    debug_btn->setCallback(std::bind(&QuakeApp::OnDebugBtnPressed, this));
    debug_btn->setTooltip("Toggle displaying mesh triangles and normals (can be slow)");
    
    screen()->performLayout();
}

void QuakeApp::OnLeftMouseDrag(const Point2 &pos, const Vector2 &delta) {
    // Optional: In our demo, we adjust the tilt of the globe here when the
    // mouse is dragged up/down on the screen.
    if (globe_mode_) {
        Matrix4 R = Matrix4::RotationY(angle);
        model_matrix = R * model_matrix;
    }
}


void QuakeApp::OnGlobeBtnPressed() {
    globe_mode();
    transform_ = true;
}

void QuakeApp::globe_mode() { 
    // Negate globe mode, sets model_matrix back to identity if rotated
    globe_mode_ = !globe_mode_; 
    model_matrix = Matrix4();
}

void QuakeApp::OnDebugBtnPressed() {
    debug_mode_ = !debug_mode_;
}

void QuakeApp::OnSliderUpdate(float value) {
    speed_box_->setValue(std::to_string((int) (value * 100)));
    playback_scale_ = 30000000.0*value;
}


void QuakeApp::UpdateSimulation(double dt)  {
    // Advance the current time and loop back to the start if time is past the last earthquake
    current_time_ += playback_scale_ * dt;
    if (current_time_ > quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds();
    }
    if (current_time_ < quake_db_.earthquake(quake_db_.min_index()).date().ToSeconds()) {
        current_time_ = quake_db_.earthquake(quake_db_.max_index()).date().ToSeconds();
    }
    
    Date d(current_time_);
    stringstream s;
    s << "Current date: " << d.month()
        << "/" << d.day()
        << "/" << d.year();
    date_label_->setCaption(s.str());
    
    angle = dt;   // Rate with which to rotate the globe on mouse drag
    
   if (transform_ && !globe_mode_) {   // globe -> plane. At plane when alpha == 1
        alpha += dt;
        alpha = GfxMath::Clamp(alpha, 0.0, 1.0);
        
        earth_.UpdateEarth(alpha);
    }

    if (transform_ && globe_mode_) {   // plane -> globe. At globe when alpha == 0
        alpha -= dt;
        alpha = GfxMath::Clamp(alpha, 0.0, 1.0);
        
        earth_.UpdateEarth(alpha);
    }
    if (alpha == 1.0 || alpha == 0.0) {
        transform_ = false;
    }
}


void QuakeApp::InitOpenGL() {
    // Set up the camera in a good position to see the entire earth in either mode
    proj_matrix_ = Matrix4::Perspective(60, aspect_ratio(), 0.1, 50);
    view_matrix_ = Matrix4::LookAt(Point3(0,0,3.5), Point3(0,0,0), Vector3(0,1,0));
    glClearColor(0.0, 0.0, 0.0, 1);
    
    // Initialize the earth object
    earth_.Init(search_path_);

    // Initialize the texture used for the background image
    stars_tex_.InitFromFile(Platform::FindFile("iss006e40544.png", search_path_));
}


void QuakeApp::DrawUsingOpenGL() {
    quick_shapes_.DrawFullscreenTexture(Color(1,1,1), stars_tex_);
    
    // You can leave this as the identity matrix and we will have a fine view of
    // the earth.  If you want to add any rotation or other animation of the
    // earth, the model_matrix is where you would apply that.
    
    // Draw the earth
    earth_.Draw(model_matrix, view_matrix_, proj_matrix_);
    if (debug_mode_) {
        earth_.DrawDebugInfo(model_matrix, view_matrix_, proj_matrix_);
    }

    // Draw earthquakes
    int quake_initial = quake_db_.FindMostRecentQuake(current_time_ - PLAYBACK_WINDOW);
    int quake_final = quake_db_.FindMostRecentQuake(current_time_);   //find the most recent earthquake's idx
    Color quake_color;
    Matrix4 scale;
    Matrix4 quake_model;
    Point3 location;

    for (int i = quake_initial; i < quake_final; i++) {
        Earthquake current_quake = quake_db_.earthquake(i);   //Get current Earthquake

        //this is a nasty one - gives location based off interpolation, definitely not optimized.
        location = earth_.LatLongToSphere(GfxMath::ToRadians(current_quake.latitude()), GfxMath::ToRadians(current_quake.longitude())).Lerp(earth_.LatLongToPlane(current_quake.latitude(), current_quake.longitude()), alpha);;
        float normalMag = (current_quake.magnitude() / quake_db_.max_magnitude());     //scale based on magnitude
        float clamp = GfxMath::Clamp(normalMag, 0.6, 0.8);

        Matrix4 translation = Matrix4::Translation(location - Point3());
        
        if (clamp <= 0.601) {   //small quake, scale and color blue. These don't show up until ~1970
            scale = Matrix4::Scale(Vector3(normalMag / 20.0, normalMag / 20.0, normalMag / 20.0));
            quake_color = Color(0.25, 0.75, 1);
        }
        else if (clamp >= 0.8) {    //big quake, scale and color red
            scale = Matrix4::Scale(Vector3(normalMag / 15.0, normalMag / 15.0, normalMag / 15.0));
            quake_color = Color(1, 0.2, 0);      
        }
        else {      //normal quake
            scale = Matrix4::Scale(Vector3(normalMag / 17.5, normalMag / 17.5, normalMag / 17.5));
            quake_color = Color(1, 0.5, 0.15);
        }
        quake_model = model_matrix * translation * scale;
        quick_shapes_.DrawSphere(quake_model, view_matrix_, proj_matrix_, quake_color);
    }
}




