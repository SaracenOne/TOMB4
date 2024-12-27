#pragma once

bool NGIsNGSavegame();

void NGWriteNGSavegameInfo();
void NGWriteNGSavegameBuffer(FILE* file);

void NGReadNGSavegameInfo();
void NGReadNGSavegameBuffer(FILE *file);