OUT=main.out
SRC=main.cpp
HEAD=helpers.hpp tokenizer.hpp ruleset.hpp

$(OUT): $(SRC) $(HEAD)
	g++ -std=c++14 -Wall -Wno-sign-compare -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
