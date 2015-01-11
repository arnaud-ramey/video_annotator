/*
 * XmlFile.h
 */
#include "XmlFile.h"
#include "video_annotator_path.h"

/*!
 * constructor
 */
XmlFile::XmlFile(string filenameXML) {
    debug("XmlFile::constructor", filenameXML);

    doc = new TiXmlDocument(filenameXML.c_str());
    bool loadOk = doc->LoadFile();
    if (!loadOk) {
        cout << "Impossible to read the file '" << filenameXML << "' ! Exiting"
                << endl;
        exit(-1);
    }
}

/*!
 * destructor
 */
XmlFile::~XmlFile() {
    debug("XmlFile::destructor");
    delete doc;
}

/*!
 * parse a xml file
 * @param filenameXML
 * @return
 */
XmlFile* XmlFile::read_xml_file(string filenameXML) {
    debug("XmlFile::read_xml_file()", filenameXML);
    XmlFile* file = new XmlFile(filenameXML);
    return file;
}

void XmlFile::save(string filename) {
    debug("XmlFile::save()", filename);
    doc->SaveFile(filename.c_str());
}

/*!
 * create a blank xml
 * @param filename_video
 * @return
 */
XmlFile* XmlFile::default_xml_file() {
    debug("XmlFile::default_xml_file()");
    XmlFile* file = new XmlFile(VIDEO_ANNOTATOR_PATH "data/blank_xml.xml");
    return file;
}

static inline string get_field(Node* node, string tag) {
    Node* searched_node = node->FirstChild(tag.c_str());
    return (searched_node->NoChildren() ? ""
            : searched_node ->FirstChild()->Value());
}

static inline int get_field_int(Node* node, string tag) {
    return atoi(get_field(node, tag).c_str());
}

static inline double get_field_double(Node* node, string tag) {
    return atof(get_field(node, tag).c_str());
}

template<class T>
static inline void set_field(Node* node, string tag, T value) {
    Node* searched_node = node->FirstChild(tag.c_str());
    // if the tag was already existing, remove its previous value
    if (searched_node)
        searched_node->Clear();
    // otherwise, create the tag
    else {
        searched_node = new TiXmlElement(tag.c_str());
        node->LinkEndChild(searched_node);
    }

    ostringstream value_as_string;
    value_as_string << value;
    TiXmlText* son = new TiXmlText(value_as_string.str().c_str());
    searched_node->LinkEndChild(son);
}

/*!
 * @param node
 * @return the number of children of the node
 */
static inline int number_of_children(Node* node, string tag) {
    Node* son = node->FirstChild(tag.c_str());
    int rep = 0;
    while (son) {
        son = son->NextSibling(tag.c_str());
        ++rep;
    }
    return rep;
}

/* implementation of the actions */
#include "Action.cpp"

/*!
 * get the filename of the video
 */
string XmlFile::get_video_filename() {
    debug("XmlFile::get_video_filename()");
    return get_field(doc_as_node(), "filename");
}

/*!
 * set the filename of the video
 */
void XmlFile::set_video_filename(string video_filename) {
    debug("XmlFile::set_video_filename()", video_filename);
    set_field(doc_as_node(), "filename", video_filename);
}

/*!
 * get the dimensions of the video
 * @param w
 * @param h
 */
void XmlFile::get_video_dimensions(int& w, int& h) {
    debug("XmlFile::get_video_dimensions()");

    Node* dimNode = doc_as_node()->FirstChild("videosize")->FirstChild(
            "imagesize");
    w = get_field_int(dimNode, "ncols");
    h = get_field_int(dimNode, "nrows");
}

/*!
 * set the dimensions of the video
 * @param w
 * @param h
 */
void XmlFile::set_video_dimensions(int w, int h) {
    debug("XmlFile::set_video_dimensions()");

    Node* dimNode = doc_as_node()->FirstChild("videosize")->FirstChild(
            "imagesize");
    set_field(dimNode, "ncols", w);
    set_field(dimNode, "nrows", h);
}

/*!
 * get the number of frames in the video
 */
int XmlFile::get_video_frame_number() {
    return get_field_int(doc_as_node()->FirstChild("videosize"), "framenumber");
}

/*!
 * set the number of frames in the video
 */
void XmlFile::set_video_frame_number(int nb) {
    set_field(doc_as_node()->FirstChild("videosize"), "framenumber", nb);
}

/*!
 * get the bitrate in the video
 */
double XmlFile::get_video_bitrate() {
    return get_field_double(doc_as_node()->FirstChild("videosize"), "bitrate");
}

/*!
 * set the bitrate in the video
 */
void XmlFile::set_video_bitrate(double bitrate) {
    set_field(doc_as_node()->FirstChild("videosize"), "bitrate", bitrate);
}

/*!
 * get the dimensions of the video in a string
 */
string XmlFile::toString_video_dimensions() {
    int w, h;
    get_video_dimensions(w, h);
    ostringstream m;
    m << w << "x" << h << ", " << get_video_frame_number();
    m << " frames @ " << get_video_bitrate() << " fps";
    return m.str();
}

/*!
 * get all the camera shots in a string
 */
void XmlFile::get_camera_shots(vector<int>* shots) {
    debug("XmlFile::get_camera_shots()");

    Node* node_shots = doc_as_node()->FirstChild("camerashots");
    // initialize the son
    Node* node_shot = node_shots->FirstChild("camerashot");
    // clear the answer
    shots->clear();

    while (1) {
        if (!node_shot)
            break;
        shots->push_back(get_field_int(node_shot, "frame_index"));
        node_shot = node_shot->NextSibling("camerashot");
    }
}

/*!
 * @param frame_index
 * @return a built Node from the index of the camera shot
 */
inline static Node* makeNode_cameraShot(int frame_index) {
    Node* node = new TiXmlElement("camerashot");
    set_field(node, "frame_index", frame_index);
    return node;
}

/*!
 * add a new camera shot
 * @param frame_index
 */
void XmlFile::add_camera_shot(int frame_index) {
    debug("XmlFile::add_camera_shot()", frame_index);

    Node* node_shots = doc_as_node()->FirstChild("camerashots");
    // initialize the son
    Node* curr_node_shot = node_shots->FirstChild("camerashot");
    Node* node_to_insert = makeNode_cameraShot(frame_index);

    while (1) {
        // if we are at the end, insert
        if (!curr_node_shot) {
            node_shots->InsertEndChild(*node_to_insert);
            break;
        }

        int curr_frame_index = get_field_int(curr_node_shot, "frame_index");
        // if there is already a camera shot, quit
        if (curr_frame_index == frame_index) {
            break;
        }
        // if the new index is bigger than ours, insert here
        if (frame_index < curr_frame_index) {
            node_shots->InsertBeforeChild(curr_node_shot, *node_to_insert);
            break;
        }
        // get the next brother
        curr_node_shot = curr_node_shot->NextSibling("camerashot");
    }
    // clean : delete the node
    delete node_to_insert;
}

/*!
 * delete a camera shot
 * @param frame_index
 */
void XmlFile::del_camera_shot(int frame_index) {
    debug("XmlFile::del_camera_shot()", frame_index);

    Node* node_shots = doc_as_node()->FirstChild("camerashots");
    // initialize the son
    Node* curr_node_shot = node_shots->FirstChild("camerashot");

    while (1) {
        // if we are at the end, break
        if (!curr_node_shot)
            break;

        int curr_frame_index = get_field_int(curr_node_shot, "frame_index");
        // if there is already a camera shot, quit
        if (curr_frame_index == frame_index) {
            node_shots->RemoveChild(curr_node_shot);
            break;
        }
        // get the next brother
        curr_node_shot = curr_node_shot->NextSibling("camerashot");
    }
}

/*!
 * get all actions
 * @param actions
 */
void XmlFile::get_actions(vector<Action>* actions) {
    debug("XmlFile::get_actions(vector<Action>* actions)");

    actions->clear();
    Node* node_actions = doc_as_node()->FirstChild("actions");
    // initialize the son
    Node* curr_node_action = node_actions->FirstChild("action");

    while (1) {
        // if we are at the end, break
        if (!curr_node_action)
            break;

        actions->push_back(Action::fromNode(curr_node_action));

        // get the next brother
        curr_node_action = curr_node_action->NextSibling("action");
    }
}

/*!
 * add a new action
 * @param action_type
 */
void XmlFile::add_action(string action_type) {
    debug("XmlFile::add_action()", action_type);

    Action to_insert(action_type);
    add_action(to_insert);
}

/*!
 * add a new action
 * @param a
 */
void XmlFile::add_action(Action& a) {
    debug("XmlFile::add_actions(Action* a)");

    Node* node_actions = doc_as_node()->FirstChild("actions");
    Node* node_to_insert = a.toNode();
    set_field(node_to_insert, "id", 1 + number_of_actions());
    node_actions->InsertEndChild(*node_to_insert);
    delete node_to_insert;
}

/*!
 * delete the action by its index
 * @param index
 */
void XmlFile::del_action(int action_id) {
    debug("XmlFile::del_action()", action_id);

    Node* node_actions = doc_as_node()->FirstChild("actions");
    Node* curr_node_action = node_actions->FirstChild("action");

    /* find the node with the good index */
    while (1) {
        // if there is no node with this index, exit
        if (!curr_node_action)
            return;
        // if we have found the good action, break
        if (get_field_int(curr_node_action, "id") == action_id)
            break;
        curr_node_action = curr_node_action->NextSibling("action");
    }

    /* if the found node is not null, delete it */
    Node* next;
    if (curr_node_action) {
        next = curr_node_action->NextSibling("action");
        debug("Type of the deleted action:",
                get_field(curr_node_action, "type"));
        node_actions->RemoveChild(curr_node_action);
    }

    /* delete the associated annotations */
    for (int frame_idx = 0; frame_idx < get_video_frame_number(); ++frame_idx) {
        del_annot(frame_idx, action_id, false);
    }

    /* reindex all the next nodes */
    while (next) {
        int old_index = get_field_int(next, "id");
        if (old_index > action_id)
            set_field(next, "id", old_index - 1);
        next = next->NextSibling("action");
    }

    /* reindex the actions in the frames */
    Node* node_frames = doc_as_node()->FirstChild("frames");
    Node* node_frame = node_frames->FirstChild("frame");
    while (node_frame) {
        Node* node_curr_annot = node_frame->FirstChild("annot");
        while (node_curr_annot) {
            // get the current annot id
            int old_curr_action_id =
                    get_field_int(node_curr_annot, "action_id");
            if (old_curr_action_id > action_id)
                set_field(node_curr_annot, "action_id", old_curr_action_id - 1);
            // go to next annot
            node_curr_annot = node_curr_annot->NextSibling("annot");

        } // end loop annot
        node_frame = node_frame->NextSibling("frame");
    } // end loop frame
}

/*!
 * get the number of actions
 */
int XmlFile::number_of_actions() {
    Node* node_actions = doc_as_node()->FirstChild("actions");
    return number_of_children(node_actions, "action");
}

/*!
 * @param frame_idx
 */
void XmlFile::create_node_of_frame(int frame_idx) {
    debug("XmlFile::create_node_of_frame()", frame_idx);

    Node* node_frames = doc_as_node()->FirstChild("frames");
    // initialize the son
    Node* curr_node_shot = node_frames->FirstChild("frame");
    Node* node_to_insert = new TiXmlElement("frame");
    set_field(node_to_insert, "index", frame_idx);

    while (1) {
        // if we are at the end, insert
        if (!curr_node_shot) {
            node_frames->InsertEndChild(*node_to_insert);
            break;
        }

        int curr_frame_index = get_field_int(curr_node_shot, "index");
        // if there is already a camera shot, quit
        if (curr_frame_index == frame_idx) {
            break;
        }
        // if the new index is bigger than ours, insert here
        if (frame_idx < curr_frame_index) {
            node_frames->InsertBeforeChild(curr_node_shot, *node_to_insert);
            break;
        }
        // get the next brother
        curr_node_shot = curr_node_shot->NextSibling("frame");
    }
    // clean : delete the node
    delete node_to_insert;
}

/*!
 *
 * @param frame_idx
 * @return
 */
Node* XmlFile::get_node_of_frame(int frame_idx) {
    //  debug("XmlFile::get_node_of_frame()", frame_idx);

    Node* frames_node = doc_as_node()->FirstChild("frames");
    Node* frame_node = frames_node->FirstChild("frame");
    while (1) {
        // son is null => return null
        if (!frame_node)
            return NULL;
        // if the good index => we found the good node !
        if (get_field_int(frame_node, "index") == frame_idx)
            return frame_node;
        // get the next
        frame_node = frame_node->NextSibling("frame");
    }
}

/*!
 * find the last keyframe where there was an annot for the given action
 * @param frame_idx
 * @param action_index
 * @param keyframe_idx -1 if not found
 * @return
 */
Annot XmlFile::prev_keyframe_annot(int frame_idx, int action_index,
        int& keyframe_idx) {
    return keyframe_annot_in_direction(frame_idx, action_index, keyframe_idx,
            -1);
}

Annot XmlFile::next_keyframe_annot(int frame_idx, int action_index,
        int& keyframe_idx) {
    return keyframe_annot_in_direction(frame_idx, action_index, keyframe_idx,
            +1);
}

Annot XmlFile::keyframe_annot_in_direction(int frame_idx, int action_index,
        int& keyframe_idx, int step) {
    for (int curr_frame_idx = frame_idx + step; curr_frame_idx >= 0
            && curr_frame_idx < get_video_frame_number(); curr_frame_idx
            += step) {
        /* get the annots */
        vector<Annot> annots;
        get_all_annots_in_frame(curr_frame_idx, &annots);

        /* look if we have the good action */
        for (vector<Annot>::iterator annot = annots.begin(); annot
                < annots.end(); ++annot) {
            // if it is the good action and keyframe => found !
            if (annot->action_id == action_index && annot->is_keyframe) {
                keyframe_idx = curr_frame_idx;
                return *annot;
            }
        }
    }

    // nothing found => return crap
    keyframe_idx = -1;
    return Annot();
}

void XmlFile::generate_annots_between_two_keyframes(int frame_idx1, Annot* a1,
        int frame_idx2, Annot* a2) {
    debug("XmlFile::generate_annots_between_two_keyframes()", frame_idx1, frame_idx2);

    assert(a1->action_id == a2->action_id);
    assert(frame_idx1 < frame_idx2);

    Interpolator inter(frame_idx1, &a1->p, frame_idx2, &a2->p);
    for (int t = frame_idx1 + 1; t < frame_idx2; ++t) {
        Polygon new_pol = inter.get_time(t);
        Annot new_annot(a1->action_id, false, new_pol);
        add_annot(t, &new_annot);
    }
}

/*!
 * add a new annot
 * @param frame_idx
 * @param annot
 */
void XmlFile::add_annot(int frame_idx, Annot* annot) {
    debug("XmlFile::add_annot(., annot)", frame_idx);

    Node* node_frame = get_node_of_frame(frame_idx);
    // if there is no node for the frame, create it
    if (!node_frame) {
        create_node_of_frame(frame_idx);
        node_frame = get_node_of_frame(frame_idx);
    } else
        // delete the old one
        del_annot(frame_idx, annot->action_id, false);

    /* add the new node */
    node_frame->LinkEndChild(annot->toNode());

    /* if keyframe, generate the other nodes */
    if (annot->is_keyframe) {
        int last_keyf, next_keyf;
        Annot next_annot = next_keyframe_annot(frame_idx, annot->action_id,
                next_keyf);
        Annot last_annot = prev_keyframe_annot(frame_idx, annot->action_id,
                last_keyf);

        // we have only one action => do nothing
        if (last_keyf == -1 && next_keyf == -1) {
        }
        // we have one action before => regenerate the actions before
        else {
            if (last_keyf != -1) {
                //cout << last_annot.toString() << endl;
                generate_annots_between_two_keyframes(last_keyf, &last_annot,
                        frame_idx, annot);
            }
            // we have one action after => regenerate the actions after
            if (next_keyf != -1) {
                generate_annots_between_two_keyframes(frame_idx, annot,
                        next_keyf, &next_annot);
            }
        } // end if last != -1 and next != -1
    } // end next_keyframe_index != -1
}

/*!
 *
 * @param frame_idx
 * @param annot
 * @return 0=no annot; 1=annot, not keyframe, 2=annot, keyframe
 */
int XmlFile::has_annot(int frame_idx, int action_index) {
    vector<Annot> annots;
    get_all_annots_in_frame(frame_idx, &annots);
    for (vector<Annot>::iterator it = annots.begin(); it < annots.end(); ++it) {
        if (it->action_id == action_index)
            return (it->is_keyframe ? 2 : 1);
    }
    return 0;
}

/*!
 * get all the annots of a given frame
 * @param frame_idx
 * @param annots
 */
void XmlFile::get_all_annots_in_frame(int frame_idx, vector<Annot>* annots) {
    //  debug("XmlFile::get_annot()", frame_idx);
    // clear the ans
    annots->clear();

    Node* node_frame = get_node_of_frame(frame_idx);
    // if there is no node for the frame, return nothing
    if (!node_frame)
        return;
    // add every annot
    Node* node_curr_annot = node_frame->FirstChild("annot");
    while (node_curr_annot) {
        annots->push_back(Annot::fromNode(node_curr_annot));
        node_curr_annot = node_curr_annot->NextSibling("annot");
    }
}

void XmlFile::del_annot(int frame_idx, int action_index, bool regenerate_annots_around) {
    debug("XmlFile::del_annot()", frame_idx, action_index);

    Node* node_frame = get_node_of_frame(frame_idx);
    // if there is no node for the frame, return nothing
    if (!node_frame)
        return;

    Node* node_curr_annot = node_frame->FirstChildElement("annot");
    bool was_keyframe;
    while (1) {
        // empty son => quit
        if (!node_curr_annot)
            return;
        // we find the good action id => remove it
        if (get_field_int(node_curr_annot, "action_id") == action_index) {
            was_keyframe = get_field_int(node_curr_annot, "is_keyframe");
            node_frame->RemoveChild(node_curr_annot);
            break;
        }
        node_curr_annot = node_curr_annot->NextSibling("annot");
    }

    // remove the annotations in the not keyframes
    if (regenerate_annots_around && was_keyframe) {
        int last_keyf, next_keyf;
        Annot last_a = prev_keyframe_annot(frame_idx, action_index, last_keyf);
        Annot next_a = next_keyframe_annot(frame_idx, action_index, next_keyf);

        // we have only one action => do nothing
        if (last_keyf == -1 && next_keyf == -1) {
        }
        // we delete the last action => delete everything from the last keyf
        else if (last_keyf != -1 && next_keyf == -1) {
            for (int t = last_keyf + 1; t < frame_idx; ++t)
                del_annot(t, action_index, false);
        }
        // we delete the first action => delete everything till the first keyf
        else if (last_keyf == -1 && next_keyf != -1) {
            for (int t = frame_idx + 1; t < next_keyf; ++t)
                del_annot(t, action_index, false);
        }
        // we delete an action in the middle => regenerate
        else if (last_keyf != -1 && next_keyf != -1) {
            generate_annots_between_two_keyframes(last_keyf, &last_a, next_keyf,
                    &next_a);
        }
    } // end of if key frame
}

/*!
 * delete all the annotations
 */
void XmlFile::del_all_anots_in_all_frames() {
    debug("XmlFile::del_all_anots_in_all_frames()");

    Node* frames_node = doc_as_node()->FirstChild("frames");
    frames_node->Clear();
}

/*
 * the print functions
 */

// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------
const static unsigned int NUM_INDENTS_PER_SPACE = 2;

const static char* getIndent(unsigned int numIndents) {
    static const char * pINDENT = "                                      + ";
    static const unsigned int LENGTH = strlen(pINDENT);
    unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
    if (n > LENGTH)
        n = LENGTH;

    return &pINDENT[LENGTH - n];
}

// same as getIndent but no "+" at the end
const static char * getIndentAlt(unsigned int numIndents) {
    static const char * pINDENT = "                                        ";
    static const unsigned int LENGTH = strlen(pINDENT);
    unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
    if (n > LENGTH)
        n = LENGTH;

    return &pINDENT[LENGTH - n];
}

static int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent) {
    if (!pElement)
        return 0;

    TiXmlAttribute* pAttrib = pElement->FirstAttribute();
    int i = 0;
    int ival;
    double dval;
    const char* pIndent = getIndent(indent);
    printf("\n");
    while (pAttrib) {
        printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

        if (pAttrib->QueryIntValue(&ival) == TIXML_SUCCESS)
            printf(" int=%d", ival);
        if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS)
            printf(" d=%1.1f", dval);
        printf("\n");
        i++;
        pAttrib = pAttrib->Next();
    }
    return i;
}

static void dump_to_stdout(Node* pParent, unsigned int indent = 0) {
    if (!pParent)
        return;

    Node* pChild;
    TiXmlText* pText;
    int t = pParent->Type();
    printf("%s", getIndent(indent));
    int num;

    switch (t) {
    case Node::DOCUMENT:
        printf("Document\n");
        break;

    case Node::ELEMENT:
        //      printf("Element [%s]", pParent->Value());
        printf("%s :", pParent->Value());

        num = dump_attribs_to_stdout(pParent->ToElement(), indent + 1);
        switch (num) {
        case 0:
            //          printf(" (No attributes)");
            break;
        case 1:
            printf("%s1 attribute\n", getIndentAlt(indent));
            break;
        default:
            printf("%s%d attributes\n", getIndentAlt(indent), num);
            break;
        }
        break;

    case Node::COMMENT:
        printf("Comment: [%s]\n", pParent->Value());
        break;

    case Node::UNKNOWN:
        printf("Unknown\n");
        break;

    case Node::TEXT:
        pText = pParent->ToText();
        //      printf("Text: [%s]\n", pText->Value());
        printf("\"%s\"\n", pText->Value());
        break;

    case Node::DECLARATION:
        printf("Declaration\n");
        break;
    default:
        break;
    }
    for (pChild = pParent->FirstChild(); pChild != 0; pChild
            = pChild->NextSibling()) {
        dump_to_stdout(pChild, indent + 1);
    }
}

void XmlFile::print() {
    debug("XmlFile::print()");
    dump_to_stdout(doc);
}

