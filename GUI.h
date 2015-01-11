/*
 * GUI.h
 *
 *  Created on: Oct 19, 2009
 *      Author: arnaud
 */

#ifndef GUI_H_
#define GUI_H_

#include "XmlFile.h"

class GUI {
public:
	static string video_filename_to_xml_filename(string video_filename);

	GUI(string filename_video = "");
	void load_file(string filename_video);
	~GUI();

	IplImage* i;
	XmlFile* xml_file;
	string filename_xml;

	int current_frame;
	void move_to_frame(int frame_index);
	void move_to_next_frame();
	void move_to_prev_frame();
	void move_to_next_keyframe();
	void move_to_prev_keyframe();

	int selected_action_id;
	CvScalar color_of_action(int action_id);

	/* main window */
	const char* window_video_name;
	CvCapture* capture;
	IplImage* frame_grabbed;
	IplImage* frame_grabbed_and_rect;
	bool is_drawing;
	CvPoint curr_rec_LU_corner;
	CvPoint curr_rec_RD_corner;
	void create_trackbar();
	void draw_annots();

	/* toolbar */
	const char* window_toolbar_name;
	IplImage* toolbar_frame;
	string* toolbar_buttons_texts;
	void create_toolbar();

	/* actions window */
	const char* window_actions_name;
	IplImage* actions_window_frame;
	double timeline_center_y, timeline_begin_x, timeline_end_x;
	void create_actions_window();
	void draw_actions_window();

	void check_video_params();
	void write_video_params(string video_filename);
	int frame_number();
	int _frame_number;

	void main_loop();

	inline void save(string filename) {
		xml_file->save(filename);
	}
	inline void save() {
		save(filename_xml);
	}
	inline void save_in_output() {
		save(filename_xml);
	}
};

static const CvScalar predefined_colors[6] = { CV_RGB(255,0,0),
		CV_RGB(0,0,255), CV_RGB(0,255,0), CV_RGB(255,0,255),
		CV_RGB(0,255,255), CV_RGB(255,255,0) };

#endif /* GUI_H_ */
