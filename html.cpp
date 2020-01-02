#include <Arduino.h>

extern const String navbar = "\
<nav> \
  <a href=\"/\"> Home </a> | \
  <a href=\"/settings\"> Settings </a> | \
  <a href=\"/debug\"> Debug </a> \
</nav> \
";

extern const String main_template = "\
<!DOCTYPE html> \
<html lang=\"en\"> \
  <head> \
    {refresh} \
    <meta charset=\"utf-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> \
    <title> Solar Panel Control </title> \
    {navbar} \
  </head> \
  {body} \
</html> \
";

extern const String homepage_body = "\
<body> \
  <br> \
  <form action=\"/\" method=\"POST\"> \
    <input type=\"hidden\" name=\"stop-att\" value=\"stop\"> \
    <input type=\"submit\" value=\"motor_stop\"> \
  </form> \
  <br> \
  <form action=\"/\" method=\"POST\"> \
    <input type=\"hidden\" name=\"auto-att\" value=\"auto\"> \
    <input type=\"submit\" value=\"motor_auto\"> \
  </form> \
  <br> \
  <form action=\"/\" method=\"POST\"> \
    <input type=\"hidden\" name=\"right-att\" value=\"right\"> \
    <input type=\"submit\" value=\"motor_right\"> \
  </form> \
  <br> \
  <form action=\"/\" method=\"POST\"> \
    <input type=\"hidden\" name=\"left-att\" value=\"left\"> \
    <input type=\"submit\" value=\"motor_left\"> \
  </form> \
  <br> \
  <br> \
  {motor_status} \
</body> \
";

extern const String notification = "\
<p style=\"color:DodgerBlue;\"> {note} </p> \
";

extern const String settings_body = "\
<body> \
  <p> This value changes the resolution received from the solar sensors acceptible values are from 1 - 64.</p> \
  <br> \
  <form class=\"form-inline\" action=\"/settings\" method=\"POST\"> \
    <input class=\"form-control\" type=\"number\" name=\"resoultion\" placeholder=\"1\"> \
    <input class=\"btn btn-primary btn-md\" type=\"submit\" value=\"Edit\"> \
  </form> \
  {res_num} \
  <p> This value changes the number of minutes for the auto motor activation. </p> \
  <br> \
  <form class=\"form-inline\" action=\"/settings\" method=\"POST\"> \
    <input class=\"form-control\" type=\"number\" step=\"0.1\" name=\"minutes\" placeholder=\"10\"> \
    <input class=\"btn btn-primary btn-md\" type=\"submit\" value=\"Edit\"> \
  </form> \
  {min_num} \
  <p> This value changes the cutoff point for the auto motor function Remeber! it correlates with the resoultion. </p> \
  <br> \
  <form class=\"form-inline\" action=\"/settings\" method=\"POST\"> \
    <input class=\"form-control\" type=\"number\" name=\"cutoff\" placeholder=\"100\"> \
    <input class=\"btn btn-primary btn-md\" type=\"submit\" value=\"Edit\"> \
  </form> \
  {cutoff_num} \
</body> \
";
extern const String debug_body = "\
<body> \
  <br> \
  {sensor_1} \
  <br> \
  {sensor_2} \
</body> \
";

//Global Values
extern String setting = "Motor on auto mode.";
extern int resolution = 1;
extern float minutes = 10.0f;
extern int cutoff = 100;
