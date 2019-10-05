#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "daemon.h"

void show_help(char *name) {
	printf("Usage:\n");
	printf("\t%s <options>\n", name);
	printf("Options:\n");
	printf("\t-s, --rpchost   Set RPC host address\n");
	printf("\t-a, --auth.     Set authentication (user:pass)\n");
	printf("\t-w, --hotwallet Set hot wallet address\n");
	printf("\t-d, --delay     (Optional) set delay time\n");
	printf("Examples:\n");
	printf("\t%s --rpchost \"http://127.0.0.1:8545\" --hotwallet \"0x...\"\n", name);
	printf("\t%s -s \"https://ropsten.infura.io\" -w \"0x...\" -d 10000\n", name);
	printf("Website: https://jesobreira.github.io/etherprise\n");
}

int main(int argc, char *argv[]) {

	char *rpchost = NULL, *hotwallet = NULL, *auth = NULL;
	int delay = 5000;

	int opt;

	const struct option args[] = {
		{"help", 		no_argument, 		0,		'h'},
		{"rpchost", 	required_argument, 	0,		's'},
		{"hotwallet", 	required_argument, 	0,		'w'},
		{"auth",		0,					0,		'a'},
		{"delay", 		0, 					0,		'd'},
		{0, 			0, 					0, 		 0 }
	};

	while((opt = getopt_long(argc, argv, "hs:w:d:", args, NULL)) > 0) {
		switch (opt) {
			case 'h':
				show_help(argv[0]);
				return 0;
			case 's':
				rpchost = optarg;
				break;
			case 'w':
				hotwallet = optarg;
				break;
			case 'd':
				delay = atoi(optarg);
				break;
		}
	}

	if (!rpchost || !hotwallet) {
		show_help(argv[0]);
		return 0;
	}

	if (etherprise_daemon_bootstrap(rpchost, hotwallet, auth) == 0)
		etherprise_daemon(rpchost, hotwallet, delay, auth);
	else
		return -1;
}