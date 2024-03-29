// generated by Fast Light User Interface Designer (fluid) version 1.0009

#include "interface.h"

Fl_Window *main_window=(Fl_Window *)0;

Fl_Button *save_button=(Fl_Button *)0;

Fl_Button *exit_button=(Fl_Button *)0;

Fl_Button *pause_button=(Fl_Button *)0;

Fl_Button *loadMotion_button=(Fl_Button *)0;

Fl_Button *loadActor_button=(Fl_Button *)0;

Fl_Button *interpolate_button=(Fl_Button *)0;

Fl_Button *locate_button=(Fl_Button *)0;

Fl_Button *rewind_button=(Fl_Button *)0;

Fl_Button *play_button=(Fl_Button *)0;

Fl_Button *repeat_button=(Fl_Button *)0;

Fl_Light_Button *record_button=(Fl_Light_Button *)0;

Fl_Value_Slider *frame_slider=(Fl_Value_Slider *)0;

Fl_Light_Button *light_button=(Fl_Light_Button *)0;

Fl_Light_Button *background_button=(Fl_Light_Button *)0;

Fl_Light_Button *edit_path_button=(Fl_Light_Button *)0;

Player_Gl_Window *glwindow=(Player_Gl_Window *)0;

Fl_Value_Input *joint_idx=(Fl_Value_Input *)0;

Fl_Value_Input *fsteps=(Fl_Value_Input *)0;

Fl_Value_Input *dt_input=(Fl_Value_Input *)0;

Fl_Value_Input *sub_input=(Fl_Value_Input *)0;

Fl_Value_Input *tx_input=(Fl_Value_Input *)0;

Fl_Value_Input *ty_input=(Fl_Value_Input *)0;

Fl_Value_Input *tz_input=(Fl_Value_Input *)0;

Fl_Value_Input *rx_input=(Fl_Value_Input *)0;

Fl_Value_Input *ry_input=(Fl_Value_Input *)0;

Fl_Value_Input *rz_input=(Fl_Value_Input *)0;

Fl_Window* make_window() {
  Fl_Window* w;
  { Fl_Window* o = main_window = new Fl_Window(741, 622, "Acclaim file player");
    w = o;
    { Fl_Group* o = new Fl_Group(10, 485, 615, 140);
      { Fl_Button* o = save_button = new Fl_Button(535, 495, 75, 40, "Save");
        o->labelsize(18);
        o->callback((Fl_Callback*)save_callback, (void*)(0));
      }
      { Fl_Button* o = exit_button = new Fl_Button(535, 545, 75, 40, "Exit");
        o->labelsize(18);
        o->callback((Fl_Callback*)exit_callback, (void*)(0));
      }
      { Fl_Button* o = pause_button = new Fl_Button(155, 560, 35, 25, "@||");
        o->labeltype(FL_SYMBOL_LABEL);
        o->labelsize(12);
        o->callback((Fl_Callback*)play_callback);
      }
      { Fl_Button* o = loadMotion_button = new Fl_Button(315, 495, 90, 40, "Load Motion");
        o->callback((Fl_Callback*)load_callback);
      }
      { Fl_Button* o = loadActor_button = new Fl_Button(315, 545, 90, 40, "Load Actor");
        o->callback((Fl_Callback*)load_callback);
      }
      { Fl_Button* o = interpolate_button = new Fl_Button(425, 545, 100, 40, "Motion Graph");
		o->callback((Fl_Callback*)interpolate_callback);
      }
      { Fl_Button* o = locate_button = new Fl_Button(320, 590, 65, 30, "Locate");
        o->callback((Fl_Callback*)locate_callback);
      }
      { Fl_Button* o = rewind_button = new Fl_Button(115, 560, 35, 25, "@|<");
        o->labeltype(FL_SYMBOL_LABEL);
        o->labelsize(12);
        o->callback((Fl_Callback*)play_callback);
      }
      { Fl_Button* o = play_button = new Fl_Button(195, 560, 35, 25, "@>");
        o->labeltype(FL_SYMBOL_LABEL);
        o->labelsize(12);
        o->callback((Fl_Callback*)play_callback, (void*)(0));
      }
      { Fl_Button* o = repeat_button = new Fl_Button(235, 560, 35, 25, "@<->");
        o->labeltype(FL_SYMBOL_LABEL);
        o->labelsize(12);
        o->callback((Fl_Callback*)play_callback);
        o->align(200|FL_ALIGN_INSIDE);
      }
      { Fl_Light_Button* o = record_button = new Fl_Light_Button(70, 560, 40, 25, "R");
        o->callback((Fl_Callback*)record_callback, (void*)(0));
      }
      o->end();
    }
    { Fl_Value_Slider* o = frame_slider = new Fl_Value_Slider(30, 535, 240, 20);
      o->type(5);
      o->labelsize(18);
      o->minimum(1);
      o->maximum(60000);
      o->step(1);
      o->callback((Fl_Callback*)fslider_callback, (void*)(0));
      o->align(197);
    }
    { Fl_Light_Button* o = light_button = new Fl_Light_Button(30, 495, 60, 25, "Light");
      o->callback((Fl_Callback*)redisplay_proc, (void*)(0));
    }
    { Fl_Light_Button* o = background_button = new Fl_Light_Button(100, 495, 105, 25, "Background");
      o->callback((Fl_Callback*)redisplay_proc, (void*)(0));
    }
	{ Fl_Light_Button* o = background_button = new Fl_Light_Button(200, 495, 105, 25, "Path Editing");
      o->callback((Fl_Callback*)switchmode_proc, (void*)(0));
    }
    { Player_Gl_Window* o = glwindow = new Player_Gl_Window(5, 5, 640, 480, "label");
      o->box(FL_DOWN_FRAME);
      o->labeltype(FL_NO_LABEL);
    }
    { Fl_Value_Input* o = joint_idx = new Fl_Value_Input(65, 595, 30, 20, "Joint");
      o->maximum(30);
      o->step(1);
      o->value(-1);
      o->callback((Fl_Callback*)valueIn_callback);
    }
    { Fl_Value_Input* o = fsteps = new Fl_Value_Input(240, 595, 30, 20, "Frame steps");
      o->minimum(1);
      o->maximum(30);
      o->step(1);
      o->value(1);
      o->callback((Fl_Callback*)valueIn_callback);
    }
	{ Fl_Value_Input* o = dt_input = new Fl_Value_Input(680, 60, 45, 25, "dt:");
      o->callback((Fl_Callback*)dt_callback);
    }
	{ Fl_Value_Input* o = sub_input = new Fl_Value_Input(680, 20, 45, 25, "sub:");
      o->callback((Fl_Callback*)sub_callback);
    }
	{ Fl_Value_Input* o = tx_input = new Fl_Value_Input(680, 95, 45, 25, "tx:");
      o->callback((Fl_Callback*)tx_callback);
    }
	{ Fl_Value_Input* o = ty_input = new Fl_Value_Input(680, 125, 45, 25, "ty:");
      o->callback((Fl_Callback*)ty_callback);
    }
	{ Fl_Value_Input* o = tz_input = new Fl_Value_Input(680, 155, 45, 25, "tz:");
      o->callback((Fl_Callback*)tz_callback);
    }
	{ Fl_Value_Input* o = rx_input = new Fl_Value_Input(680, 190, 45, 25, "rx:");
      o->callback((Fl_Callback*)rx_callback);
    }
	{ Fl_Value_Input* o = ry_input = new Fl_Value_Input(680, 220, 45, 25, "ry:");
      o->callback((Fl_Callback*)ry_callback);
    }
	{ Fl_Value_Input* o = rz_input = new Fl_Value_Input(680, 250, 45, 25, "rz:");
      o->callback((Fl_Callback*)rz_callback);
    }
    o->end();
  }
  return w;
}
