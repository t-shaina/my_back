Приложение представляет собой TCP-сервер, служащий для обработки запросов к базе данных. 
Входные данные принимаются в JSON формате.  В качестве СУБД использована PostrgeSQL. 
Для сборки требуется  cmake (version 3.14), gcc (version 11.4.0), Qt6 (Core, Sql, Network).
Использование
укажите дерево сборки: cmake -S <src_path>  -B <build_path>
запустите сборку проекта: cmake --build <build_path>
