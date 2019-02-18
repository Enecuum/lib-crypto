# lib-crypto

Elliptic curve cryptography library.

Сейчас данный код демонстрирует работу алгоритмов 4-8 из [TechPaper](https://new.enecuum.com/files/tp_en.pdf), а также проверку подписи по формуле 3.5
Для работы необходима библиотека OpenSSL (на момент написания использована [Win32 OpenSSL v1.1.1](https://slproweb.com/products/Win32OpenSSL.html).

For Linux:

`sudo apt-get install libssl-dev`

Сборка:
```
cd crypto
g++ *.cpp -lcrypto -o test
./test
```
При возникновении ошибки **"No openssl_applink"** следует собрать проект с опцией /MD