#include <string>

struct Place {
    std::string place_name;
    double lat;
    double lon;
};

struct WeatherRegistration {
    uint16_t id;
    std::string date;
    std::string time;
    Place place;
    float temp;
    uint8_t hum;
};

WeatherRegistration reg = {
    0, 
    "20211105",
    "12:15",
    {
        "Aarhus N",
        13.692,
        19.438,
    },
    13.1,
    70,
};
