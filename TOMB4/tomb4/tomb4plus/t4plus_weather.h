#pragma once

enum WeatherType {
	WEATHER_DISABLED,
	WEATHER_ENABLED_IN_SPECIFIC_ROOMS,
	WEATHER_ENABLED_ALL_OUTSIDE
};

extern WeatherType rain_type; // TRLE
extern WeatherType snow_type; // TRLE

extern long rain_outside; // TRLE
extern long snow_outside; // TRLE

// TRLE - Weather effects
extern void InitWeatherFX();
extern void ClearWeatherFX();
extern void DoWeather();