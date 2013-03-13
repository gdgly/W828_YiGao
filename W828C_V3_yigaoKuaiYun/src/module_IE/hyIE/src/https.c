/**
 * SSL �Ľӿں���,��Ҫ����https
 * Ŀǰ��OPENSSL����Ϥ���ݲ���ȫ�ֱ������б���ssl�ľ���������ý�������һ��ssl���輰ʱ�ͷ�
 */
#include "https.h"
#ifdef HAVE_SSL


static SSL_CTX *gpSSL_ctx = NULL;    //SSL������

/**
 * ��ʼ��SSL������
 */
int protocol_https_init(void)
{
    Ssl_InitToSdram();      //��ʼ��static var
	SSL_library_init();
	//ERR_load_ERR_strings();
    gpSSL_ctx = SSL_CTX_new(SSLv23_client_method()) ;
    if (gpSSL_ctx == NULL)
    	return HYWB_ERR;
	return HYWB_OK;
}

/**
 * �ͷ���������Դ
 */
int protocol_https_destroy(void)
{
	SSL_CTX_free(gpSSL_ctx);
	CRYPTO_cleanup_all_ex_data();
	EVP_cleanup();
	//ERR_free_strings();
	return HYWB_OK;
}

/**
 * ��ȡSSL������
 */
SSL_CTX *protocol_https_get_ssl_ctx(void)
{
	return gpSSL_ctx;
}

/**
 * �ͷ�ssl��Դ
 */
int protocol_https_close(int sock, SSL *ssl)
{
	int ret;
    ret = SSL_shutdown(ssl);
    if (ret != 1) {
        SSL_shutdown(ssl);
    }
    closesocket(sock);
    SSL_free(ssl);
    return HYWB_OK;
}

/**
 * ssl������������
 */
SSL *protocol_https_connect(int sock)
{
    SSL *ssl;
    SSL_CTX *ctx;
    int ret;
   
	ctx = protocol_https_get_ssl_ctx();
    ssl = SSL_new(ctx);
    ret = SSL_set_fd(ssl, sock);   //��socket��SSL����
    if (ret == 0) {
        SSL_free(ssl);
        return NULL;
    }
    
    RAND_poll();
    while (RAND_status() == 0) {
        unsigned short rand_ret = rand() % 65536;
        RAND_seed(&rand_ret, sizeof(rand_ret));
    }
    
    ret = SSL_connect(ssl);
    if (ret != 1) {
        SSL_free(ssl);
        return NULL;    
    }
    return ssl;
}


#endif