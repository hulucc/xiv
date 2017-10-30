// XIV.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include "Memory.h"
#include "Process.h"
#include "FFXIV.h"
#include "SignatureScanner.h"
#include "Signatures.h"
#include "Pointers.h"
#include "Player.h"
#include "Structs.h"


void initialize_ffxiv_data() {
	/*
		Updates ffxiv singleton with process information.
	*/
	FFXIV *ffxiv = FFXIV::instance();

	// Get data
	HWND hwnd = get_hwnd(ffxiv->window_name);
	DWORD pid = get_pid(hwnd);
	HANDLE handle = get_process_handle(pid);
	MODULEENTRY32 module = get_module(pid, ffxiv->process_name);

	// Update instance
	ffxiv->set_hwnd(hwnd);
	ffxiv->set_pid(pid);
	ffxiv->set_handle(handle);
	ffxiv->set_module(module);
}

void find_all_signatures(FFXIV *ffxiv) {
	SignatureScanner *scanner = new SignatureScanner();
	for (auto &signature : all_signatures) {
		BYTE* result = scanner->find_pattern(
			ffxiv->get_handle(),
			ffxiv->get_module(),
			signature->pattern, 
			signature->mask
		);
		signature->address = result + signature->offset;
	}
}

void find_all_pointers(FFXIV *ffxiv) {
	player_base->address = read_address_pointers<BYTE*>(
		ffxiv->get_handle(),
		ffxiv->get_base_address(),
		player_base->offsets
	);
}

int main()
{
	FFXIV *ffxiv = FFXIV::instance();
	MemoryRegistry *registry = MemoryRegistry::instance();
	initialize_ffxiv_data();

	//find_all_signatures(ffxiv);
	find_all_pointers(ffxiv);
	Player thicc_heaux = Player(player_base->address);
	//thicc_heaux.position = Position(player_cords->address);
	//thicc_heaux.parameters = Parameters(player_parameters->address);
	registry->scan();
	return 0;
}
