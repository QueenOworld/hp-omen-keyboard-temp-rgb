#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

typedef struct rgb_color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb_color;

typedef struct hsv_color {
    unsigned char h;
    unsigned char s;
    unsigned char v;
} hsv_color;

rgb_color hsvtorgb(hsv_color hsv) {
    rgb_color rgb;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0) {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
    case 0:
        rgb.r = hsv.v;
        rgb.g = t;
        rgb.b = p;
        break;
    case 1:
        rgb.r = q;
        rgb.g = hsv.v;
        rgb.b = p;
        break;
    case 2:
        rgb.r = p;
        rgb.g = hsv.v;
        rgb.b = t;
        break;
    case 3:
        rgb.r = p;
        rgb.g = q;
        rgb.b = hsv.v;
        break;
    case 4:
        rgb.r = t;
        rgb.g = p;
        rgb.b = hsv.v;
        break;
    default:
        rgb.r = hsv.v;
        rgb.g = p;
        rgb.b = q;
        break;
    }

    return rgb;
}

unsigned long rgbtoulong(unsigned char r, unsigned char g, unsigned char b) {
    return (r << 16) + (g << 8) + b;
}

int main() {
    while (true) {
        std::ifstream thermal0("/sys/class/thermal/thermal_zone0/temp");
        std::ifstream thermal1("/sys/class/thermal/thermal_zone1/temp");

        std::ofstream zone00("/sys/devices/platform/hp-wmi/rgb_zones/zone00");
        std::ofstream zone01("/sys/devices/platform/hp-wmi/rgb_zones/zone01");
        std::ofstream zone02("/sys/devices/platform/hp-wmi/rgb_zones/zone02");
        std::ofstream zone03("/sys/devices/platform/hp-wmi/rgb_zones/zone03");

        std::string temp0;
        std::string temp1;

        thermal0 >> temp0;
        thermal1 >> temp1;

        int average = (stoi(temp0) + stoi(temp1)) / 2;

        hsv_color color_hsv =
            hsv_color{(unsigned char)(348 - 4 * (average / 1000)), 255, 255};

        rgb_color color_rgb = hsvtorgb(color_hsv);

        unsigned long color = rgbtoulong(color_rgb.r, color_rgb.g, color_rgb.b);

        std::ostringstream stream;
        stream << std::hex << color;
        std::string output(stream.str());

        zone00 << output;

        zone01 << output;

        zone02 << output;

        zone03 << output;

        sleep(2);
    }
}
