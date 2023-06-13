#include "../inc/server.h"

char* rsaKeyToString(RSA* rsaKey) {
    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio) return NULL;

    if (!PEM_write_bio_RSAPublicKey(bio, rsaKey)) {
        BIO_free(bio);
        return NULL;
    }

    char* keyString = NULL;
    long keyLength = BIO_get_mem_data(bio, &keyString);
    char* result = malloc(keyLength + 1);
    if (!result) {
        BIO_free(bio);
        return NULL;
    }

    memcpy(result, keyString, keyLength);
    result[keyLength] = '\0';
    BIO_free(bio);

    return result;
}

RSA* handle_key_exchange(int socket, KeyPair server_key_pair, char *command) {
    BIO* client_key_bio = BIO_new(BIO_s_mem());
    BIO_write(client_key_bio, command, strlen(command));
    RSA* client_rsa_key = PEM_read_bio_RSAPublicKey(client_key_bio, NULL, NULL, NULL);
    if (client_rsa_key == NULL) {
        fprintf(stderr, "Error: unable to load RSA key\n");
        BIO_free(client_key_bio);
        exit(1);
    }

    BIO* server_key_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(server_key_bio, server_key_pair.public_key);
    int server_key_len = BIO_pending(server_key_bio);
    char* server_key_str = malloc(server_key_len + 1);
    BIO_read(server_key_bio, server_key_str, server_key_len);
    server_key_str[server_key_len] = '\0';
    send(socket, server_key_str, server_key_len, 0);

    BIO_free(server_key_bio);
    BIO_free(client_key_bio);
    return client_rsa_key;
}

char* mx_key_request_check(int socket) {
    char buffer_for_key[1024] = {0};
    int n_for_key = read(socket, buffer_for_key, 1024);
    if (n_for_key == 0) {
        printf("Key error\n");
        return NULL;
    }
    char **command = mx_strsplit(buffer_for_key, '|');
    if (mx_strcmp(command[0], "ke") == 0) {
        return command[1];
    }
    else {
        return NULL;
    }
}

RSA* string_to_rsa_key(const char* key_str) {
    RSA* rsa_key = NULL;
    BIO* bio = NULL;

    bio = BIO_new_mem_buf(key_str, -1);
    if (bio == NULL) {
        fprintf(stderr, "Error creating BIO from key string\n");
        return NULL;
    }

    rsa_key = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (rsa_key == NULL) {
        fprintf(stderr, "Error loading RSA key from string\n");
        BIO_free(bio);
        return NULL;
    }

    BIO_free(bio);
    return rsa_key;
}

KeyPair mx_generate_key_pair() {
    KeyPair key_pair;
    key_pair.private_key = RSA_new();
    BIGNUM *bne = BN_new();
    int bits = 2048;
    unsigned long e = RSA_F4;
    BN_set_word(bne, e);

    RSA_generate_key_ex(key_pair.private_key, bits, bne, NULL);
    key_pair.public_key = RSAPublicKey_dup(key_pair.private_key);

    BN_free(bne);

    return key_pair;
}


