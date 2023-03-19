Сборка плагина из каталога со сборкой
make SetImplicit

Использование плагина (аккуратно с адресами)
./bin/clang++ -Xclang -ast-dump -E -fplugin=lib/PrintClasses.so -fplugin-arg-set-ai main.cpp

