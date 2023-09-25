# OpenQUACK: проект ПО для портативных радиостанций Quansheng UV-K5/UV-K5(8)/UV-5R PLUS/UV-K6
А также Anysecu UV-K5, Radtel RT-590 и так далее  

__ОТПРАВИТЬ БЛАГОДАРОЧКУ НА ПИВО: 2204 1201 0314 6060__  
[Канал в Telegram](https://t.me/openquack)  

![OpenQUACK](images/openquack.png)

Основан на коде https://github.com/DualTachyon и https://github.com/fagci  

__Несовместим со штатным CPS, требует установки [кастомного модуля CHIRP](https://github.com/rebezhir/openquack-chirp-driver) !!!__  
На время работы над прошивкой самой рации доработка модуля приостановлена.  

![OpenQUACK](images/work_in_progress.jpg)  
ПО рассчитано на использование рации именно как средства связи, при этом средства удобного.

Поэтому удаляется все ненужное:
* Экран приветствия, дающий секундную задержку при загрузке
* Прием FM-радио, для боковой клавиши в настройках CHIRP заменено на переключение A/B (также доступно через меню рации)
* Передача сигнала тревоги, для боковой клавиши в настройках CHIRP и заменено на переключение VFO/MR (также доступно через меню рации)
* Прием NOAA по причине бесполезности в России
* Копирование по воздуху
* Киллкоды
* Соответствующие убираемым функциям пункты меню
* Озвучивание меню
  

# Добавлено: 
* Редактирование настроек боковых клавиш из меню рации  

![OpenQUACK](images/keys.png)    

* Увеличено усиление микрофона
* Расширен диапазон настройки длительности подсветки
* Горизонтальное меню V1.1  

![OpenQUACK](images/menu.png)  
* Расширенная блокировка кнопок (только цифровые / все, кроме PTT / все)  
* Переключение TDR по F+0 вместо радио  
* Встроенная калибровка вольтметра в скрытом меню (PTT+F1 при включении рации)

#ПОРЯДОК КАЛИБРОВКИ:
1. __Полностью__ зарядить батарею. При калибровке автоматически прописывается именно та настройка, при которой показания АЦП контроллера будут соответствовать 8,4 В.
2. Войти в инженерное меню рации, включив ее с одновременно нажатыми PTT и верхней боковой клавишей.
3. Перейти к пункту 60 меню.
4. Однократно нажать кнопку MENU.
5. Выключить рацию и снова включить.


# TODO:
* Русифицированное горизонтальное меню  

![OpenQUACK](images/sample.gif)  

* Расширенный редактор в меню (названия каналов, сканлисты, список контактов и т.д.)

  




