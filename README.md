# lib-crypto

Elliptic curve cryptography library.

Сейчас данный код демонстрирует работу алгоритмов 4-8 из [TechPaper](https://new.enecuum.com/files/tp_en.pdf), а также проверку подписи по формуле 3.5
Для работы необходима библиотека OpenSSL 

OpenSSL for Linux:

`sudo apt-get install libssl-dev`

Сборка примера:
```
cd crypto
g++ *.cpp -lcrypto -o test
./test
```
Сборка .so библиотеки:
```
cd crypto
g++ -fPIC *.cpp -shared -o libecc.so
```

### Node.js Binding

Для использования библиотеки в связке с Nodejs написан биндинг-аддон на основе [node-addon-api](https://github.com/nodejs/node-addon-api). 
**Проблемы с работой в Windows 10. Причины выясняются.**

Сборка аддона:
В файле `binding.gyp` указать верный путь к библиотеке libecc.so
```
...
	"libraries": [ "../lib-crypto/crypto/libecc.so" ]
...
```
В файле `node-bignumber.h` указать верный путь к заголовку `crypto.h`

`#include "../lib-crypto/crypto.h"`

Сборка и запуск примера
```
cd node-addon
npm install
node ./addon.js
```