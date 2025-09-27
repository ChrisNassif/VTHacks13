all:
	sudo g++ -Ofast foxx_driver.cpp -o /usr/bin/foxx_driver.o

test:
	sudo g++ -Ofast foxx_test_driver.cpp -o /usr/bin/foxx_test_driver.o