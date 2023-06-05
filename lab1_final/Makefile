REPORTER = bin/reporter.exe
CREATOR = bin/creator.exe
MAIN = bin/main.exe
run: $(REPORTER) $(CREATOR) $(MAIN)
	./$(MAIN)
$(REPORTER): reporter.cpp
	g++ reporter.cpp -o $(REPORTER)
$(CREATOR): creator.cpp
	g++ creator.cpp -o $(CREATOR)
$(MAIN): main.cpp
	g++ main.cpp -o $(MAIN)
clean:
	-rm bin/*