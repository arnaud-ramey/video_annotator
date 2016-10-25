/*
 * XmlFile.h
 *
 *  Created on: Oct 19, 2009
 */

#ifndef XMLFILE_H_
#define XMLFILE_H_

#define DEBUG 1
#define DEFAULT_OUTPUT_DIR "output/"

#define USE_DEFAULT_OUTPUT_DIR 1
// otherwise, it uses the same directory as the source video

// STL imports
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

// openCV imports
#include <opencv2/opencv.hpp>

// TinyXml imports
#include "tinyxml/tinyxml.h"
typedef TiXmlNode Node;

/*!
 * the representation of an action
 */
class Action {
public:
  static string ANSWERPHONE, DRIVECAR, EAT, FIGHTPERSON, GETOUTCAR,
      HANDSHAKE, HUGPERSON, KISS, RUN, SITDOWN, SITUP, STANDUP;
  string type;
  bool deleted;
  bool verified;
  string username;

  Action(string action_type);
  ~Action();

  string toString();
  Node* toNode();
  static Action fromNode(Node* src);
};

/*!
 * a simple polygon
 */
class Polygon {
public:
  vector<CvPoint2D32f> points;
  Polygon() {
  }
  Polygon(CvPoint2D32f p1, CvPoint2D32f p2, CvPoint2D32f p3, CvPoint2D32f p4) {
    points.push_back(p1);
    points.push_back(p2);
    points.push_back(p3);
    points.push_back(p4);
  }
  Polygon(double x1, double y1, double x2, double y2, double x3, double y3,
      double x4, double y4) {
    points.push_back(cvPoint2D32f(x1, y1));
    points.push_back(cvPoint2D32f(x2, y2));
    points.push_back(cvPoint2D32f(x3, y3));
    points.push_back(cvPoint2D32f(x4, y4));
  }
  Node* toNode();
  static Polygon fromNode(Node* src);

  string toString() {
    ostringstream rep;
    rep << "points :" << endl;
    for (vector<CvPoint2D32f>::iterator pt = points.begin(); pt
        < points.end(); ++pt)
      rep << " (" << pt->x << ", " << pt->y << ")";
    return rep.str();
  }
};

#include "Interpolator.h"

/*!
 * an annotation on a frame
 */
class Annot {
public:
  int action_id;
  bool is_keyframe;
  string date;
  Polygon p;

  Annot() {}
  Annot(int action_id, bool iskey, Polygon p);
  Node* toNode();
  static Annot fromNode(Node* src);
  string toString() {
    ostringstream rep;
    rep << "action_id:" << action_id;
    rep << ", is_keyframe:" << is_keyframe;
    rep << ", date:" << date;
    rep << ", p:" << p.toString();
    return rep.str();
  }
};

/*!
 * the representation of the XML file
 */
class XmlFile {
public:
  ~XmlFile();
  static XmlFile* read_xml_file(string filenameXML);
  static XmlFile* default_xml_file();
  void save(string filename);

  string get_video_filename();
  void set_video_filename(string video_filename);
  void get_video_dimensions(int& w, int& h);
  void set_video_dimensions(int w, int h);
  int get_video_frame_number();
  void set_video_frame_number(int nb);
  double get_video_bitrate();
  void set_video_bitrate(double bitrate);
  string toString_video_dimensions();

  void get_camera_shots(vector<int>* shots);
  void add_camera_shot(int frame_index);
  void del_camera_shot(int frame_index);

  void get_actions(vector<Action>* actions);
  void add_action(Action& a);
  void add_action(string action_type);
  void del_action(int action_id);
  int number_of_actions();

  void add_annot(int frame_idx, Annot* annot);
  int has_annot(int frame_idx, int action_index);
  void get_all_annots_in_frame(int frame_idx, vector<Annot>* annots);
  void del_annot(int frame_idx, int action_index, bool regenerate_annots_around);
  void del_all_anots_in_all_frames();

  void print();

  //private:
  XmlFile(string filenameXML);
  TiXmlDocument* doc;
  TiXmlNode* doc_as_node() {
    return ((TiXmlNode*) doc)->FirstChildElement();
  }
  Node* get_node_of_frame(int frame_idx);
  void create_node_of_frame(int frame_idx);

  Annot
  prev_keyframe_annot(int frame_idx, int action_index, int& keyframe_idx);
  Annot
  next_keyframe_annot(int frame_idx, int action_index, int& keyframe_idx);
  Annot
  keyframe_annot_in_direction(int frame_idx, int action_index, int& keyframe_idx,
      int step);

  void generate_annots_between_two_keyframes(int frame_idx1, Annot* a1, int frame_idx2, Annot* a2);
};

/*!
 * a display function, in debug mode
 * @param s string to display
 */
inline static void debug(string s) {
#if DEBUG
  cout << "!!!" << s << endl;
#endif
}
template<class T>
inline static void debug(string s, T t) {
#if DEBUG
  cout << "!!!" << s << " = " << t << endl;
#endif
}
template<class T, class T2>
inline static void debug(string s, T t, T2 t2) {
#if DEBUG
  cout << "!!!" << s << " = " << t << ", " << t2 << endl;
#endif
}

#endif /* XMLFILE_H_ */
