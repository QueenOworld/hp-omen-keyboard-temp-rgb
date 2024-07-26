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
    return ((r) << 16) + ((g) << 8) + (b);
}

int main() {
    while (true) {
        std::fstream thermal0("/sys/class/thermal/thermal_zone0/temp",
                              std::ios_base::in);
        std::fstream thermal1("/sys/class/thermal/thermal_zone1/temp",
                              std::ios_base::in);

        std::string temp0;
        std::string temp1;

        if (thermal0.is_open()) {
            thermal0 >> temp0;
            thermal0.close();
        }

        if (thermal1.is_open()) {
            thermal1 >> temp1;
            thermal1.close();
        }

        int average = (stoi(temp0) + stoi(temp1)) / 2;

        std::ofstream zone00("/sys/devices/platform/hp-wmi/rgb_zones/zone00");
        std::ofstream zone01("/sys/devices/platform/hp-wmi/rgb_zones/zone01");
        std::ofstream zone02("/sys/devices/platform/hp-wmi/rgb_zones/zone02");
        std::ofstream zone03("/sys/devices/platform/hp-wmi/rgb_zones/zone03");

        hsv_color color_hsv =
            hsv_color{(unsigned char)(348 - 4 * (average / 1000)), 255, 255};

        rgb_color color_rgb = hsvtorgb(color_hsv);

        unsigned long color = rgbtoulong(color_rgb.r, color_rgb.g, color_rgb.b);

        std::stringstream stream;
        stream << std::hex << color;
        std::string output(stream.str());

        if (zone00.is_open()) {
            zone00 << output;
            zone00.close();
        }
        if (zone01.is_open()) {
            zone01 << output;
            zone01.close();
        }
        if (zone02.is_open()) {
            zone02 << output;
            zone02.close();
        }
        if (zone03.is_open()) {
            zone03 << output;
            zone03.close();
        }

        sleep(1);
    }
}
