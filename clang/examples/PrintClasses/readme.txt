Сборка плагина из папки со сборкой
make PrintClasses

Использование плагина (аккуратно с адресами)
bin/clang -cc1 -load ./lib/PrintClasses.so -fdelayed-template-parsing -plugin print-c main.cpp
