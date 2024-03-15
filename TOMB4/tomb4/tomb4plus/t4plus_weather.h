#pragma once

enum T4OverrideFogMode {
	T4_FOG_DEFAULT,
	T4_FOG_FORCE_VOLUMETRIC,
	T4_FOG_FORCE_DISTANT
};

extern T4OverrideFogMode t4_override_fog_mode;

enum T4WeatherType {
	WEATHER_DISABLED,
	WEATHER_ENABLED_IN_SPECIFIC_ROOMS,
	WEATHER_ENABLED_ALL_OUTSIDE
};

extern T4WeatherType t4_rain_type; // TRLE
extern T4WeatherType t4_snow_type; // TRLE

extern long rain_outside; // TRLE
extern long snow_outside; // TRLE

// TRLE - Weather effects
extern void InitWeatherFX();
extern void ClearWeatherFX();
extern void DoWeather();