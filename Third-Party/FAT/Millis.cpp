/**
 * @file Millis.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
 * Universidade de Passo Fundo (http://www.upf.br/)
 * 
 * @copyright
 * Copyright (c) 2018 Angelo Elias Dalzotto & Gabriel Boni Vicari
 * 
 * @brief The Millis class manages the milliseconds timer.
 */

#include "Millis.h"

bool Millis::initialized = false;
uint32_t Millis::counter = 0;

void Millis::init()
{
    if(initialized)
        return;
    initialized = true;


}

uint32_t Millis::get()
{

    return Timer::SysTime;
}
