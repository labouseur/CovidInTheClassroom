#ifndef MAIN_H
#define MAIN_H

#include <QTextStream>

QTextStream out(stdout);

int main(int argc, char* argv[]);
void welcomeMessage();
int parseAndRun(char* argv[]);

#endif // MAIN_H
