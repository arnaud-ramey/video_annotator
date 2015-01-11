/*
 * GUI.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: arnaud
 */

#include "GUI.h"
#include "XmlFile.h"

string GUI::video_filename_to_xml_filename(string video_filename) {
	string rep = video_filename;
	// remove video extension
	rep = rep.substr(0, rep.length() - 3);
	// adding xml extension
	rep = rep + "xml";

#if USE_DEFAULT_OUTPUT_DIR
	// find the '/'
	size_t real_filename_begin = rep.find_last_of("/");
	string real_filename;
	if (real_filename_begin != string::npos)
		real_filename = rep.substr(1 + real_filename_begin);
	else
		real_filename = rep;
	//	cout << "real_filename:" << real_filename << endl;
	rep = (string) DEFAULT_OUTPUT_DIR + real_filename;
#endif
	return rep;
}

/*!
 * constructor
 * @param filename_video
 */
GUI::GUI(string filename_video /* = "" */) {
	debug("GUI::constructor - video", filename_video);

	if (filename_video.length() > 0)
		load_file(filename_video);

	/* create the window */
	window_video_name = filename_video.c_str();
	cvNamedWindow(window_video_name, CV_WINDOW_AUTOSIZE);
	create_toolbar();
	create_actions_window();
	create_trackbar();
	move_to_frame(0);

	/* move the windows */
	int padding = 30;
	cvMoveWindow(window_toolbar_name, 0, 0);
	cvMoveWindow(window_actions_name, padding + toolbar_frame->width, 0);
	cvMoveWindow(window_video_name, padding + toolbar_frame->width, padding
			+ actions_window_frame->height);

	/* launch */
	if (filename_video.length() > 0)
		main_loop();
}

/*!
 * load a new video
 * @param filename_video
 */
void GUI::load_file(string filename_video) {
	debug("GUI::load_file()", filename_video);

	/* open the video */
	capture = cvCreateFileCapture(filename_video.c_str());
	_frame_number = -1;
	if (capture == NULL) {
		cout << "Impossible to open the video '" << filename_video << "'. ";
		cout << "Exiting." << endl;
		exit(-1);
	}

	/* try to open the associated xml */
	filename_xml = video_filename_to_xml_filename(filename_video);
	debug("filename_xml", filename_xml);
	FILE *fp = fopen(filename_xml.c_str(), "r");
	// XML file exits => open it
	if (fp) {
		xml_file = XmlFile::read_xml_file(filename_xml);
		// check the dimensions
		fclose(fp);
	}
	// XML file does not exist => create a blank
	else {
		xml_file = XmlFile::default_xml_file();
		write_video_params(filename_video);
	}

	check_video_params();

	// init the selected action id
	selected_action_id
			= (xml_file && xml_file->number_of_actions() > 0 ? 1 : 0);
}

/*!
 * destructor
 */
GUI::~GUI() {
	debug("GUI::destructor");

	delete xml_file;
	cvReleaseCapture(&capture);
	cvDestroyWindow(window_video_name);
	cvReleaseImage(&frame_grabbed_and_rect);

	//	delete[] toolbar_buttons_texts;
	cvReleaseImage(&toolbar_frame);
	cvDestroyWindow(window_toolbar_name);

	cvReleaseImage(&actions_window_frame);
}

/*!
 * check if the dimensions of the loaded videos are the same than
 * the one in the XML file
 */
void GUI::check_video_params() {
	debug("GUI::check_video_params()");
	// redim the frame grabbed if the size changed
	int frame_w = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frame_h = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	if (frame_grabbed_and_rect && (frame_w != frame_grabbed_and_rect->width
			|| frame_h != frame_grabbed_and_rect->height)) {
		cvReleaseImage(&frame_grabbed_and_rect);
	}
	debug("frame_w, h", frame_w, frame_h);
	debug("frame number:", frame_number());
}

/*!
 * write in the xml file the dimensions of the video
 * @param video_filename
 */
void GUI::write_video_params(string video_filename) {
	debug("GUI::write_video_params()", video_filename);

	// filename
	xml_file->set_video_filename(video_filename);
	// fps
	double bitrate = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	xml_file->set_video_bitrate(bitrate);
	// image number
	double frame_w = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double frame_h = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	xml_file->set_video_frame_number(frame_number());
	xml_file->set_video_dimensions(frame_w, frame_h);
}

/*!
 * get the number of frames in the video
 * @return
 */
int GUI::frame_number() {
	//return 20;
		return cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	/* if the number of frames was never computed, find it*/
	if (_frame_number == -1) {
		CvCapture* capture2 = cvCreateFileCapture(
				xml_file->get_video_filename().c_str());
		frame_grabbed = cvQueryFrame(capture2);

		while (frame_grabbed) {
			frame_grabbed = cvQueryFrame(capture2);
			++_frame_number;
		}

		cvReleaseCapture(&capture2);
	}

	return _frame_number;

}

/*!
 * the move to frame function, out of the GUI class
 * (necessary for the trackbar)
 * @param frame_index
 * @param callback
 */
void move_to_frame2(int frame_index, void* callback) {
	debug("move_to_frame2()", frame_index);

	GUI* gui = (GUI*) callback;
	if (!gui->capture)
		return;

	// change the current frame index
	gui->current_frame = frame_index;

	// move the video
	cvSetCaptureProperty(gui->capture, CV_CAP_PROP_POS_FRAMES, frame_index);

	// refresh frame
	gui->frame_grabbed = cvQueryFrame(gui->capture);
	// and the rectangle version
	if (!gui->frame_grabbed_and_rect)
		gui->frame_grabbed_and_rect = cvCloneImage(gui->frame_grabbed);

	// load annotations
	gui->draw_annots();

	// redraw timeline
	gui->draw_actions_window();
}

/*!
 * change the current frame
 * @param frame_index the new frame index
 */
void GUI::move_to_frame(int frame_index) {
	move_to_frame2(frame_index, this);
	//move trackbar
	//	current_frame = -1;
	//	cvSetTrackbarPos("Position", window_video_name, frame_index);
}

/*!
 * goto the next frame
 */
void GUI::move_to_next_frame() {
	if (current_frame < frame_number() - 1)
		move_to_frame(current_frame + 1);
}

/*!
 * goto the previous frame
 */
void GUI::move_to_prev_frame() {
	if (current_frame > 0)
		move_to_frame(current_frame - 1);
}

/*!
 * goto the next frame
 */
void GUI::move_to_next_keyframe() {
	int next_keyf;
	xml_file->next_keyframe_annot(current_frame, selected_action_id, next_keyf);
	if (next_keyf != -1)
		move_to_frame(next_keyf);
}

/*!
 * goto the next frame
 */
void GUI::move_to_prev_keyframe() {
	int prev_keyf;
	xml_file->prev_keyframe_annot(current_frame, selected_action_id, prev_keyf);
	if (prev_keyf != -1)
		move_to_frame(prev_keyf);
}

/*!
 * @param action_id
 * @return the color associated with the given action
 */
CvScalar GUI::color_of_action(int action_id) {
	return predefined_colors[(action_id - 1) % 6];
}

/*!
 * the callback for the main window
 */
void main_window_callback(int event, int x, int y, int flags, void* param) {
	GUI* gui = (GUI*) param;

	/* if there is no action selected, do nothing */
	if (gui->selected_action_id <= 0 || gui->selected_action_id
			> gui->xml_file->number_of_actions())
		return;

	/* if we press the left button => start a current rectangle */
	if (event == CV_EVENT_LBUTTONDOWN) {
		// clear an old annot
		gui->xml_file->del_annot(gui->current_frame, gui->selected_action_id,
				false);
		gui->draw_annots();

		gui->is_drawing = true;
		gui->curr_rec_LU_corner.x = x;
		gui->curr_rec_LU_corner.y = y;
	}

	/* if we lift the left button => add the action */
	if (event == CV_EVENT_LBUTTONUP) {
		gui->is_drawing = false;
		// get the coords of the rectangle
		double up = gui->curr_rec_LU_corner.y;
		double left = gui->curr_rec_LU_corner.x;
		double down = gui->curr_rec_RD_corner.y;
		double right = gui->curr_rec_RD_corner.x;
		if (up > down)
			swap(up, down);
		if (left > right)
			swap(left, right);

		// add the rectangle
		Polygon p(left, up, right, up, right, down, left, down);
		Annot a(gui->selected_action_id, true, p);
		gui->xml_file->add_annot(gui->current_frame, &a);

		// redraw the timeline
		gui->draw_actions_window();
	}

	/* if we double click with the right button => delete the annot */
	if (event == CV_EVENT_RBUTTONDBLCLK) {
		/// check if there is an annot and if it a keyframe
		if (gui->xml_file->has_annot(gui->current_frame,
				gui->selected_action_id) != 2)
			return;

		// clear an old annot
		gui->xml_file->del_annot(gui->current_frame, gui->selected_action_id,
				true);
		gui->draw_annots();

		// redraw the timeline
		gui->draw_actions_window();
	}

	/* if drawing, keep the current mouse positon */
	if (gui->is_drawing) {
		gui->curr_rec_RD_corner.x = x;
		gui->curr_rec_RD_corner.y = y;
	}
}

/*!
 * create the trackbar of the window, to move in the video
 */
void GUI::create_trackbar() {
	// putting frame at 1 to create a trackbar event
	current_frame = 1;
	//	cvCreateTrackbar2("Position", window_video_name, &current_frame,
	//			frame_number() - 1, move_to_frame2, this);

	// set the callback
	cvSetMouseCallback(window_video_name, main_window_callback, this);
}

int ICON_WIDTH = 75, ICON_HEIGHT = 75;
int ICONS_PER_LINE = 3;
#define ICONS_NUMBER 12

/*!
 *
 * insert an image into another
 * @param where_to_insert
 * @param filename_to_be_inserted
 * @param x, y where to insert
 * @param w, h size of the image to insert
 *
 */
void insert_image(IplImage* where_to_insert, string filename_to_be_inserted,
		int x, int y, int w, int h) {
	ostringstream filename_full;
	filename_full << "images/" << filename_to_be_inserted << ".png";
	IplImage* button_big = cvLoadImage(filename_full.str().c_str(),
			CV_LOAD_IMAGE_COLOR);
	IplImage* button = cvCreateImage(cvSize(w, h), 8, 3);
	cvResize(button_big, button);
	cvSetImageROI(where_to_insert, cvRect(x, y, w, h));
	cvCopy(button, where_to_insert);
	cvResetImageROI(where_to_insert);
	cvReleaseImage(&button_big);
	cvReleaseImage(&button);
}

/*!
 * the callback for the toolbar
 */
void toolbar_callback(int event, int x, int y, int flags, void* param) {
	GUI* gui = (GUI*) param;

	if (event == CV_EVENT_LBUTTONDOWN) {
		/* find the name of the pressed button */
		int button_i = x / ICON_WIDTH;
		int button_j = y / ICON_HEIGHT;
		int button_idx = button_j * ICONS_PER_LINE + button_i;
		string button_txt = gui->toolbar_buttons_texts[button_idx];

		//		cout << x << "," << y << endl;
		//		cout << "button_idx:" << button_idx << endl;
		cout << "button:" << button_txt << endl;

		/* insert the action */
		gui->xml_file->add_action(button_txt);
		gui->selected_action_id = gui->xml_file->number_of_actions();
		gui->draw_actions_window();
	}
}

/*!
 * create the window for the toolbar
 */
void GUI::create_toolbar() {
	debug("GUI::create_toolbar()");

	int help_line_height = 15;
	int help_strings_size = 2;
	string help[] = { "o/p:prev/next image", "l/m:prev/next key image" };

	/* create toolbar image */
	int toolbar_width = ICON_WIDTH * ICONS_PER_LINE;
	int toolbar_height = ICONS_NUMBER * ICON_HEIGHT / ICONS_PER_LINE;
	toolbar_height += help_line_height * help_strings_size + 10;
	toolbar_frame = cvCreateImage(cvSize(toolbar_width, toolbar_height), 8, 3);

	/* insert the name of the buttons */
	string txt[ICONS_NUMBER] = { Action::ANSWERPHONE, Action::DRIVECAR,
			Action::EAT, Action::FIGHTPERSON, Action::GETOUTCAR,
			Action::HANDSHAKE, Action::HUGPERSON, Action::KISS, Action::RUN,
			Action::SITDOWN, Action::SITUP, Action::STANDUP };
	toolbar_buttons_texts = new string[ICONS_NUMBER];

	/* import short buttons */
	for (int button_id = 0; button_id < ICONS_NUMBER; ++button_id) {
		toolbar_buttons_texts[button_id] = txt[button_id];
		int x = (button_id % ICONS_PER_LINE) * ICON_WIDTH;
		int y = (button_id / ICONS_PER_LINE) * ICON_HEIGHT;
		insert_image(toolbar_frame, toolbar_buttons_texts[button_id], x, y,
				ICON_WIDTH, ICON_HEIGHT);
	}

	/* print the help */
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1, CV_AA);
	string toolbar_help = "test\ntest2";
	int x = 10;
	for (int line_nb = 1; line_nb <= help_strings_size; ++line_nb) {
		int y = ((ICONS_NUMBER - 1) / ICONS_PER_LINE + 1) * ICON_HEIGHT
				+ help_line_height * line_nb;
		cvPutText(toolbar_frame, help[line_nb - 1].c_str(), cvPoint(x, y),
				&font, cvScalar(255, 255, 255));
	}

	/* create window */
	window_toolbar_name = "toolbar";
	cvNamedWindow(window_toolbar_name, CV_WINDOW_AUTOSIZE);
	//	cvMoveWindow(window_video_name, toolbar_width + 50, 0);
	cvShowImage(window_toolbar_name, toolbar_frame);

	/* add the mouse callback */
	cvSetMouseCallback(window_toolbar_name, toolbar_callback, this);
}

/*!
 * the callback for the toolbar
 */
void actions_window_callback(int event, int x, int y, int flags, void* param) {
	GUI* gui = (GUI*) param;

	if (event == CV_EVENT_LBUTTONDOWN || event == CV_EVENT_RBUTTONDBLCLK) {
		int button_i = x / ICON_WIDTH;
		int button_j = y / ICON_HEIGHT;
		int button_idx = button_i + gui->xml_file->number_of_actions()
				* button_j;
		button_idx += 1; // the actions start with id = 1

		/* select the chosen action if needed */
		if (button_idx <= gui->xml_file->number_of_actions()) {
			if (event == CV_EVENT_RBUTTONDBLCLK) {
				gui->xml_file->del_action(button_idx);
				gui->selected_action_id = 0;
			}
			if (event == CV_EVENT_LBUTTONDOWN) {
				gui->selected_action_id = button_idx;
			}
			gui->draw_actions_window();
		} // end of if button idx OK

		/* move the time line according to the click if needed */
		else if (button_j == 1 && x >= gui->timeline_begin_x && x
				< gui->timeline_end_x) {
			int chosen_frame_idx = 1.f * gui->frame_number() * (x
					- gui->timeline_begin_x) / (gui->timeline_end_x
					- gui->timeline_begin_x);
			//cout << x << ", " << chosen_frame_idx << endl;
			gui->move_to_frame(chosen_frame_idx);
		}
	} // end of if button down
}

/*!
 * create the window for the toolbar
 */
void GUI::create_actions_window() {
	debug("GUI::create_actions_window()");

	/* create image for the window */
	actions_window_frame = cvCreateImage(
			cvSize(8 * ICON_WIDTH, 2 * ICON_HEIGHT), 8, 3);

	/* create window */
	window_actions_name = "actions";
	cvNamedWindow(window_actions_name, CV_WINDOW_AUTOSIZE);

	/* add the mouse callback */
	cvSetMouseCallback(window_actions_name, actions_window_callback, this);

	/* coords of the zone */
	timeline_center_y = 1.5 * ICON_HEIGHT;
	timeline_begin_x = 10;
	timeline_end_x = actions_window_frame->width - 15;

	draw_actions_window();
}

/*!
 * refresh the window for the toolbar
 */
void GUI::draw_actions_window() {
	debug("GUI::draw_actions_window()");

	if (!xml_file)
		return;

	/* get current actions */
	vector<Action> actions;
	xml_file->get_actions(&actions);

	/* draw actions */
	cvZero(actions_window_frame);
	int current_action_id = 0;
	for (vector<Action>::iterator it = actions.begin(); it < actions.end(); ++it) {
		insert_image(actions_window_frame, it->type, current_action_id
				* ICON_HEIGHT, 0, ICON_WIDTH, ICON_HEIGHT);
		++current_action_id;
	}

	/* draw a rectanle around the actions */
	//	cout << "selected action id :" << selected_action_id << endl;
	for (int action_id = 1; action_id <= xml_file->number_of_actions(); ++action_id) {
		CvPoint begin_corner = cvPoint((action_id - 1) * ICON_WIDTH, 0);
		CvPoint end_corner = cvPoint(action_id * ICON_WIDTH, ICON_HEIGHT);
		int thickness = (action_id == selected_action_id ? 15 : 2);
		cvRectangle(actions_window_frame, begin_corner, end_corner,
				color_of_action(action_id), thickness);
	}

	/* draw the timeline for the current action */
	// clear the previous
	cvRectangle(actions_window_frame, cvPoint(0, ICON_HEIGHT), cvPoint(
			actions_window_frame->width, actions_window_frame->height),
			CV_RGB(50,50,50), -1);

	for (int frame_idx = 0; frame_idx < frame_number(); ++frame_idx) {
		CvPoint center = cvPoint(timeline_begin_x + frame_idx * timeline_end_x
				/ frame_number(), timeline_center_y);

		// draw a vertical line for the current frame
		if (frame_idx == current_frame)
			cvLine(actions_window_frame, cvPoint(center.x, ICON_HEIGHT),
					cvPoint(center.x, 2 * ICON_HEIGHT), CV_RGB(255,255,255), 2);

		// draw the circles
		int has_annot = xml_file->has_annot(frame_idx, selected_action_id);
		if (has_annot > 0)
			cvCircle(actions_window_frame, center, 2 * has_annot,
					color_of_action(selected_action_id), -1);
		else
			cvCircle(actions_window_frame, center, 1 + 2 * has_annot,
					CV_RGB(255,255,255), -1);
	}

	/* show image */
	cvShowImage(window_actions_name, actions_window_frame);
}

/*!
 * draw the annots on the current frame
 */
void GUI::draw_annots() {
	debug("GUI::draw_annots()");
	/* get the annots */
	vector<Annot> annots;
	xml_file->get_all_annots_in_frame(current_frame, &annots);

	/* draw the annots */
	cvCopy(frame_grabbed, frame_grabbed_and_rect);

	for (vector<Annot>::iterator it = annots.begin(); it < annots.end(); ++it) {
		vector<CvPoint2D32f>* pts = &it->p.points;
		int nb_pt = pts->size();

		// copy the points in an array
		CvPoint pts_arr[nb_pt];
		CvPoint* pts_arr_ptr = pts_arr;
		for (vector<CvPoint2D32f>::iterator pt = pts->begin(); pt < pts->end(); ++pt) {
			pts_arr_ptr->x = (int) pt->x;
			pts_arr_ptr->y = (int) pt->y;
			++pts_arr_ptr;
		}
		pts_arr_ptr = pts_arr;

		// call the polygon function
		int thickness = (it->is_keyframe ? 4 : 2);
		cvPolyLine(frame_grabbed_and_rect, &pts_arr_ptr, &nb_pt, 1, 1,
				color_of_action(it->action_id), thickness);
	}
}

/*!
 * the main loop of the GUI
 */
void GUI::main_loop() {
	debug("GUI::main_loop()");

	while (1) {
		// refresh the rect if needed
		if (is_drawing) {
			draw_annots();
			cvRectangle(frame_grabbed_and_rect, curr_rec_LU_corner,
					curr_rec_RD_corner, color_of_action(selected_action_id), 3);
		}

		cvShowImage(window_video_name, frame_grabbed_and_rect);

		// keys
		char c = cvWaitKey(33);
		//cout << "c:'" << c << "'" << endl;
		if (c == 27)
			break;
		if (c == 'p')
			move_to_next_frame();
		if (c == 'o')
			move_to_prev_frame();
		if (c == 'l')
			move_to_prev_keyframe();
		if (c == 'm')
			move_to_next_keyframe();

	}
}
