CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lm -lncurses
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/database.c $(SRC_DIR)/depenses.c $(SRC_DIR)/revenus.c $(SRC_DIR)/epargnes.c $(SRC_DIR)/bilan_mensuel.c $(SRC_DIR)/multi_devises.c $(SRC_DIR)/transactions_recurrentes.c $(SRC_DIR)/budget.c $(SRC_DIR)/recherche_filtrage.c $(SRC_DIR)/rapports_graphiques.c $(SRC_DIR)/analyse_tendances.c $(SRC_DIR)/previsions_financieres.c $(SRC_DIR)/cryptage_donnees.c $(SRC_DIR)/export_import.c $(SRC_DIR)/synchronisation_locale.c $(SRC_DIR)/ia_simple.c $(SRC_DIR)/interface.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/planificateur

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

run: $(EXECUTABLE)
	@mkdir -p $(DATA_DIR)
	./$(EXECUTABLE)

.PHONY: all clean run