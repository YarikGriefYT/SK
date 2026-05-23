# C++ Messenger (VK-like)

Простой мессенджер с клиент-серверной архитектурой на C++ (сокеты, многопоточность).

## Возможности
- Регистрация / логин
- Личные сообщения с офлайн-доставкой
- Список друзей
- История переписки
- Просмотр онлайн-пользователей

## Сборка и запуск

### Требования
- Linux (или WSL на Windows)
- g++ с поддержкой C++17
- make (опционально)

### Компиляция

```bash
# Клонируем репозиторий
git clone https://github.com/ваш_username/cpp-messenger.git
cd cpp-messenger

# Сборка через make
make

# Или вручную:
g++ server.cpp -o server -std=c++17 -pthread
g++ client.cpp -o client -std=c++17 -pthread