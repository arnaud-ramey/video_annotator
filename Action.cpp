/*
 * Action.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: arnaud
 */

string Action::ANSWERPHONE = "answer_phone";
string Action::DRIVECAR = "drive_car";
string Action::EAT = "eat";
string Action::FIGHTPERSON = "fight_person";
string Action::GETOUTCAR = "get_out_car";
string Action::HANDSHAKE = "hand_shake";
string Action::HUGPERSON = "hug_person";
string Action::KISS = "kiss";
string Action::RUN = "run";
string Action::SITDOWN = "sit_down";
string Action::SITUP = "sit_up";
string Action::STANDUP = "stand_up";

/*!
 * constructor
 */
Action::Action(string action_type) {
    //  debug("Action::constructor");

    type = action_type;
    deleted = 0;
    verified = 1;
    username = "Sobhan_Arnaud";
}

/*!
 * destructor
 */
Action::~Action() {
    //  debug("Action::destructor");
}

/*!
 * transform into node
 * @return
 */
Node* Action::toNode() {
    Node* ans = new TiXmlElement("action");
    set_field(ans, "type", type);
    set_field(ans, "deleted", deleted);
    set_field(ans, "verified", verified);
    set_field(ans, "username", username);
    return ans;
}

/*!
 * constructor from node
 */
Action Action::fromNode(Node* src) {
    Action ans(get_field(src, "type"));
    return ans;
}

/*!
 * @return a string version
 */
string Action::toString() {
    ostringstream m;
    m << "Type:'" << type << "', ";
    m << "Deleted:'" << deleted << "', ";
    m << "Verified:'" << verified << "', ";
    m << "Username:'" << username << "', ";

    return m.str();
}

/*!
 * transform into node
 * @return
 */
Node* Polygon::toNode() {
    Node* ans = new TiXmlElement("polygon");
    for (vector<CvPoint2D32f>::iterator it = points.begin(); it < points.end(); ++it) {
        Node* node_pt = new TiXmlElement("pt");
        set_field(node_pt, "x", it->x);
        set_field(node_pt, "y", it->y);
        ans->LinkEndChild(node_pt);
    }
    return ans;
}

/*!
 * constructor from node
 */
Polygon Polygon::fromNode(Node* src) {
    Polygon p;
    Node* son = src->FirstChild("pt");
    while (son) {
        double x = get_field_double(son, "x");
        double y = get_field_double(son, "y");
        p.points.push_back(cvPoint2D32f(x, y));
        son = son->NextSibling();
    }
    return p;
}

Node* Annot::toNode() {
    Node* ans = new TiXmlElement("annot");
    set_field(ans, "action_id", action_id);
    set_field(ans, "is_keyframe", is_keyframe);
    set_field(ans, "date", date);
    ans->LinkEndChild(p.toNode());
    return ans;
}

Annot::Annot(int action_id, bool iskey, Polygon p) {
    this->action_id = action_id;
    this->is_keyframe = iskey;
    this->p = p;
    // set the time
    time_t theTime;
    time(&theTime); // get the calendar time
    tm *t = localtime(&theTime); // convert to local
    string date_with_newline = asctime(t);
    this->date = date_with_newline.substr(0, date_with_newline.length() - 1);
}

Annot Annot::fromNode(Node* src) {
    Annot annot(//
            get_field_int(src, "action_id"),//
            get_field_int(src, "is_keyframe"),//
            Polygon::fromNode(src->FirstChild("polygon"))//
            );
    annot.date = get_field(src, "date");
    return annot;
}
