#Трансформер [![Build Status](https://travis-ci.org/SPC-project/Transformer.svg?branch=master)](https://travis-ci.org/SPU-project/Transformer)
##Осуществляет переход от двумерного разбиения треугольниками к разбиению прямыми призмами

Проект собирается с помощью системы автоматизированной сборки [CMake](http://www.cmake.org/). Для получения бинарного файла вам необходимо: 

1. [Подготовить проект](http://www.cmake.org/Wiki/CMake_Generator_Specific_Information), сгенерировав файл управления сборкой (make, Visual Studio, Code::Blocks, прочее). <i>Совет:</i> не забудьте указать тип сборки (debug/release). Пример (Linux, терминал): `cmake . -DCMAKE_BUILD_TYPE=Release`
2. Собрать проект, используя посредством используемой системы сборки
