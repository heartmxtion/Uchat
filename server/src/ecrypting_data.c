#include "../inc/server.h"

char* mx_encrypting_data(const char *data, RSA* client_key) {
    int rsa_len = RSA_size(client_key);
    unsigned char* encrypted_auth = malloc(rsa_len);
    int encrypted_len = RSA_public_encrypt(mx_strlen(data), (unsigned char *)data, encrypted_auth, client_key, RSA_PKCS1_PADDING);
    if (encrypted_len == -1) {
        fprintf(stderr, "Error, failed to encrypt login and password\n");
        exit(1);
    }

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO *bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, encrypted_auth, encrypted_len);
    BIO_flush(b64);
    BUF_MEM *bufferPtr;
    BIO_get_mem_ptr(b64, &bufferPtr);
    char *result = malloc(bufferPtr->length + 1);
    memcpy(result, bufferPtr->data, bufferPtr->length);
    result[bufferPtr->length] = '\0';

    BIO_free_all(b64);
    free(encrypted_auth);
    
    return result;
}

char *mx_decrypting_data(char *buffer, RSA* private_key) {
    // Декодирование base64
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* bmem = BIO_new_mem_buf((void*)buffer, -1);
    bmem = BIO_push(b64, bmem);
    int rsa_len = RSA_size(private_key);
    unsigned char* encrypted = malloc(rsa_len);
    int len = BIO_read(bmem, encrypted, rsa_len);
    BIO_free_all(bmem);
    if (len < rsa_len) {
        fprintf(stderr, "Error: RSA auth decryption failed (BIO_read serv)\n");
        exit(1);
    }

    unsigned char* decrypted = malloc(rsa_len);
    int decrypted_len = RSA_private_decrypt(rsa_len, encrypted, decrypted, private_key, RSA_PKCS1_PADDING);
    if (decrypted_len == -1) {
        fprintf(stderr, "Error: RSA auth decryption failed\n");
        exit(1);
    }

    char* result = malloc(decrypted_len + 1);
    memcpy(result, decrypted, decrypted_len);
    result[decrypted_len] = '\0';

    free(decrypted);
    free(encrypted);

    return result;
}
