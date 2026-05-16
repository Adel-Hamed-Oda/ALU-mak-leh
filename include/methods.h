#pragma once

#include "include.h"

void add();
void sub();
void mul();
void ldi();
void beqz();
void and();
void or();
void jr();
void sal();
void sar();
void lb();
void sb();

void display_register_content(int idx);
void display_memory_content(int idx);
void display_instruction_content(int idx);