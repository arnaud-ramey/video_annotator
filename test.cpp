/*
 * test.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: arnaud
 */
#include "Interpolator.h"
#include "XmlFile.h"
#include "GUI.h"

#include <iomanip>

static void test_read_xml(string filenameXML) {
  XmlFile* file = XmlFile::read_xml_file(filenameXML);
  file->print();
  delete file;
}

/*!
 * tests for the filenames
 */
static void test_XML_filenames() {
  XmlFile* file = XmlFile::default_xml_file();
  //  file->print();

  cout << "file->get_video_filename():" << file->get_video_filename() << endl;
  file->set_video_filename("toto.avi");
  cout << "file->get_video_filename():" << file->get_video_filename() << endl;
  file->set_video_filename("toto2.avi");
  cout << "file->get_video_filename():" << file->get_video_filename() << endl;

  //  file->print();
  file->save("out.xml");
  delete file;
}

/*!
 * tests for the dimensiosn of the video
 */
static void test_XML_dims() {
  XmlFile* file = XmlFile::default_xml_file();

  cout << "file dim :" << file->toString_video_dimensions() << endl;

  file->set_video_dimensions(100, 150);
  file->set_video_frame_number(20);
  file->set_video_bitrate(24.976);
  cout << "file dim :" << file->toString_video_dimensions() << endl;

  //  file->print();
  file->save("out.xml");
  delete file;
}

/*!
 * tests for the camera shots
 */
static void test_XML_shots() {
  XmlFile* file = XmlFile::default_xml_file();

  file->add_camera_shot(10);
  file->add_camera_shot(27);
  file->add_camera_shot(13);
  file->add_camera_shot(1);
  file->add_camera_shot(27);
  file->del_camera_shot(13);
  file->del_camera_shot(18);
  vector<int> shots;
  file->get_camera_shots(&shots);
  cout << "shots:";
  for (vector<int>::iterator it = shots.begin(); it < shots.end(); ++it)
    cout << *it << ",";
  cout << endl;

  //  file->print();
  file->save("out.xml");
  delete file;
}

/*!
 * test on the actions
 */
static void test_XML_actions() {
  XmlFile* file = XmlFile::default_xml_file();

  file->add_action(Action::ANSWERPHONE);
  file->add_action(Action::DRIVECAR);
  file->add_action(Action::HANDSHAKE);
  file->add_action(Action::ANSWERPHONE);
  file->add_action(Action::EAT);
  file->add_action(Action::ANSWERPHONE);
  file->del_action(2);
  file->del_action(2);
  file->del_action(3);
  vector<Action> actions;
  file->get_actions(&actions);
  cout << "actions:";
  for (vector<Action>::iterator it = actions.begin(); it < actions.end(); ++it)
    cout << it->type << ", ";
  cout << endl;

  //  file->print();
  file->save("out.xml");
  delete file;
}

/*!
 * the annotations
 */
static void test_XML_annotations() {
  XmlFile* file = XmlFile::default_xml_file();
  file->set_video_frame_number(10);

  /* add some actions */
  file->add_action(Action::ANSWERPHONE);
  file->add_action(Action::DRIVECAR);
  file->add_action(Action::HANDSHAKE);
  file->add_action(Action::FIGHTPERSON);

  /* add annots */
  Annot a1(2, true, Polygon(1, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(3, &a1);
  Annot a2(4, true, Polygon(2, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(3, &a2);
  Annot a3(3, true, Polygon(3, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(7, &a3);
  cout << endl;
  Annot a4(2, true, Polygon(4, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(5, &a4);

  /* replace 1 with a new polygon */
  cout << endl;
  Annot a5(2, true, Polygon(5, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(3, &a5);

  // delete 2
  cout << endl;
  file->del_annot(3, 4, true);
  // delete an unused action -> reindex the annt
  cout << endl;
  file->del_action(1);
  // delete a used action -> delete the associated annots
  cout << endl;
  file->del_action(2); // previous action 3

  //  file->print();
  file->save("out.xml");
  delete file;
}

static void test_interpolator() {
  Polygon p1(1, 1, 1, 2, 2, 2, 2, 1);
  Polygon p2(2, 2, 2, 3, 4, 3, 4, 2);
  Interpolator inter(0, &p1, 10, &p2);

  cout << "p:" << inter.get_time(0).toString() << endl;
  cout << "p:" << inter.get_time(2).toString() << endl;
  cout << "p:" << inter.get_time(10).toString() << endl;
  cout << "p:" << inter.get_time(20).toString() << endl;
}

static void test_interpolator2() {
  XmlFile* file = XmlFile::default_xml_file();
  file->set_video_frame_number(20);

  /* add some actions */
  file->add_action(Action::ANSWERPHONE);
  file->add_action(Action::DRIVECAR);

  /* add annots */
  Annot a1(2, true, Polygon(1, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(1, &a1);
  cout << endl;
  Annot a2(2, true, Polygon(4, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(4, &a2);
  cout << endl;
  Annot a3(2, true, Polygon(8, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(8, &a3);
  cout << endl;
  Annot a4(2, true, Polygon(100, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(6, &a4);
  cout << endl;
  Annot a5(2, true, Polygon(10, 0, 0, 0, 0, 0, 0, 0));
  file->add_annot(10, &a5);
  cout << endl;

  file->del_annot(6, 2, true);
  cout << endl;
  file->del_annot(10, 2, true);
  cout << endl;

  /* find the keyframes */
  //      int keyf;
  //      file->last_keyframe_annot(3, 2, keyf);
  //      cout << "keyf:" << keyf << endl;

  file->save("out.xml");
  delete file;
}

static void test_GUI(string filename_video) {
  //  cout << "xml name:" << GUI::video_filename_to_xml_filename(filename_video)
  //          << endl;
  GUI* gui = new GUI(filename_video);

  gui->save();
  //  gui->save("out.xml");
  delete gui;
}

static int sequence_GUI(string prefix, int begin_index, int end_index,
                        int leading_zeros, string suffix) {
  GUI* gui = new GUI();
  int idx;

  for (idx = begin_index; idx < end_index; ++idx) {
    ostringstream filename;
    filename << prefix;
    filename << setfill('0') << setw(leading_zeros) << idx;
    filename << suffix;

    cout << endl << endl;
    debug("filename", filename.str());
    gui->load_file(filename.str());
    // refresh display
    gui->move_to_frame(0);

    gui->main_loop();

    // when it is over, save
    gui->save();
  } // end loop idx

  delete gui;
}

int main(int argc, char **argv) {
  debug("main");
  if (argc != 2) {
    printf("Synopsis:%s VIDEOFILE\n", argv[0]);
    printf("\n");
    printf("Use:\n");
    printf("Go to the frame you want to annotate,\n"
           "select the action you want in the toolbar,"
           "and draw a rectangle in the image\n"
           "This creates a keyframe.\n");
    printf("Action ROIs (rectangles) are interpolated between keyframes.\n");
    printf("\n");
    printf("Keys:\n");
    printf(" * 'o': move to previous frame\n");
    printf(" * 'n': move to next     frame\n");
    printf(" * 'o': move to previous keyframe\n");
    printf(" * 'n': move to next     keyframe\n");
    return -1;
  }
  //  test_read_xml("data/sample.xml");

  //  test_XML_filenames();
  //  test_XML_dims();
  //  test_XML_shots();
  //  test_XML_actions();
  //  test_XML_annotations();

  //  test_interpolator();
  //  test_interpolator2();

  string video_filename = argv[1];
      // "../data/city_lights.avi";
  //"../videos/AVIClips/actioncliptest00100.avi";
  test_GUI(video_filename);

  //sequence_GUI("../videos/AVIClips/actioncliptest", 36, 100, 5, ".avi");
  //sequence_GUI("../videos/AVIClips/actioncliptrain", 1, 100, 5, ".avi");
  return 0;
}
