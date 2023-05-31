// #include <enc_ops.h>
// #include <request_types.h>
// #include <stdio.h>
// #include <sys/stat.h>

// int handle_ops(BaseRequest *base_req)
// {
//     bool found = false;
//     // DMSG("\n -------------------\nops: %d", base_req->reqType);

//     switch (base_req->reqType)
//     {
//     case CMD_INT_PLUS:
//     case CMD_INT_MINUS:
//     case CMD_INT_MULT:
//     case CMD_INT_DIV:
//     case CMD_INT_EXP:
//     case CMD_INT_MOD:
//         base_req->resp = enc_int32_calc((EncIntCalcRequestData *)base_req);
//         break;
//     case CMD_INT_CMP:
//         base_req->resp = enc_int32_cmp((EncIntCmpRequestData *)base_req);
//         break;
//     case CMD_INT_SUM_BULK:
//         base_req->resp = enc_int32_bulk((EncIntBulkRequestData *)base_req);
//         break;

//     case CMD_INT_ENC:{
//         EncIntEncRequestData *req = (EncIntEncRequestData *) base_req;
//         // printf("enc request %d\n", req->plaintext);
//         base_req->resp = encrypt_bytes((uint8_t*) &req->plaintext, sizeof(req->plaintext),
//                                   (uint8_t*) &req->ciphertext, sizeof(req->ciphertext));
//         break;
//     }
//     case CMD_INT_DEC:{
//         EncIntDecRequestData *req = (EncIntDecRequestData *) base_req;
//         base_req->resp = decrypt_bytes((uint8_t*)&req->ciphertext, sizeof(req->ciphertext),
//                                   (uint8_t*)&req->plaintext, sizeof(req->plaintext));
//         break;
//     }

//     /* FLOATs */

//     case CMD_FLOAT_PLUS:
//     case CMD_FLOAT_MINUS:
//     case CMD_FLOAT_MULT:
//     case CMD_FLOAT_DIV:
//     case CMD_FLOAT_EXP:
//     case CMD_FLOAT_MOD:
//         base_req->resp = enc_float32_calc((EncFloatCalcRequestData *)base_req);
//         break;

//     case CMD_FLOAT_CMP:
//         base_req->resp = enc_float32_cmp((EncFloatCmpRequestData *)base_req);
//         break;

//     case CMD_FLOAT_SUM_BULK:
//         base_req->resp = enc_float32_bulk((EncFloatBulkRequestData *) base_req);
//         break;

//     case CMD_FLOAT_EVAL_EXPR:
//         base_req->resp = enc_float32_eval_expr((EncFloatEvalExprRequestData *)base_req);

//     case CMD_FLOAT_ENC:{
//         EncFloatEncRequestData *req = (EncFloatEncRequestData *) base_req;
//         req->common.resp = encrypt_bytes((uint8_t*) &req->plaintext, sizeof(req->plaintext),
//                                         (uint8_t *) &req->ciphertext, sizeof(req->ciphertext));
//         break;
//     }
//     case CMD_FLOAT_DEC:{
//         EncFloatDecRequestData *req = (EncFloatDecRequestData *) base_req;
//         req->plaintext = float_map_find(f_map_p, &req->ciphertext, &found);
//         if (!found) {
//             req->common.resp = decrypt_bytes((uint8_t *) &req->ciphertext, sizeof(req->ciphertext),
//                                         (uint8_t*) &req->plaintext, sizeof(req->plaintext));
//         }
//         break;
//     }

//     /* timestamp*/
//     case CMD_TIMESTAMP_EXTRACT_YEAR:
//         base_req->resp = enc_timestamp_extract_year((EncTimestampExtractYearRequestData *)base_req);
//         break;
//     case CMD_TIMESTAMP_CMP:
//         base_req->resp = enc_timestamp_cmp((EncTimestampCmpRequestData *)base_req);
//         break;
//     case CMD_TIMESTAMP_ENC:{
//         EncTimestampEncRequestData *req = (EncTimestampEncRequestData *) base_req;
//         req->common.resp = encrypt_bytes((uint8_t*) &req->plaintext, sizeof(req->plaintext),
//                                         (uint8_t *) &req->ciphertext, sizeof(req->ciphertext));
//         break;
//     }
//     case CMD_TIMESTAMP_DEC:{
//         EncTimestampDecRequestData *req = (EncTimestampDecRequestData *) base_req;
//         req->common.resp = decrypt_bytes((uint8_t *) &req->ciphertext, sizeof(req->ciphertext),
//                                         (uint8_t*) &req->plaintext, sizeof(req->plaintext));
//         break;
//     }

//     /* text */

//     case CMD_STRING_SUBSTRING:
//         base_req->resp = enc_text_substring((SubstringRequestData *)base_req);
//         break;
//     case CMD_STRING_CONCAT: // like use calc data, because return enctext value.

//         base_req->resp = enc_text_concatenate((EncStrCalcRequestData *)base_req);
//         break;
//     case CMD_STRING_LIKE: // like use cmp data, because return boolean value.
//         base_req->resp = enc_text_like((EncStrCmpRequestData *)base_req);
//         break;
//     case CMD_STRING_CMP:
//         base_req->resp = enc_text_cmp((EncStrCmpRequestData *)base_req);
//         break;
//     case CMD_STRING_ENC:{
//         EncStrEncRequestData *req = (EncStrEncRequestData *) base_req;
//         req->ciphertext.len = req->plaintext.len + IV_SIZE + TAG_SIZE;
//         req->common.resp = encrypt_bytes((uint8_t*) &req->plaintext.data, req->plaintext.len,
//                                         (uint8_t *) &req->ciphertext.enc_cstr, req->ciphertext.len);
//         break;
//     }
//     case CMD_STRING_DEC:{
//         EncStrDecRequestData *req = (EncStrDecRequestData *) base_req;
//         Str dec_res;
//         dec_res = text_map_find(t_map_p, &req->ciphertext, &found);
//         if (found) {
//             dec_res.data[dec_res.len] = '\0';
//             memcpy(&req->plaintext, &dec_res, sizeof(Str));
//             // assert(dec_res.len == req->ciphertext.len - IV_SIZE - TAG_SIZE)
//         } else {
//             req->plaintext.len = req->ciphertext.len - IV_SIZE - TAG_SIZE;
//             req->common.resp = decrypt_bytes((uint8_t *) &req->ciphertext.enc_cstr, req->ciphertext.len,
//                                             (uint8_t*) &req->plaintext.data, req->plaintext.len);
//             req->plaintext.data[req->plaintext.len] = '\0';
//             text_map_insert(t_map_p, &req->ciphertext, &req->plaintext);
//         }
//         break;
//     }
//     default:
//         break;
//     }

//     return 0;
// }

// int main(int argc, char *argv[]){

//     char buffer[sizeof(EncIntBulkRequestData)];
//     /* first, get a buffer containing the encrypted request
//         the buffer can be read from a file.
//     or  read from a shared memory  */
//     if(argc != 2){
//         printf("should input a filename as parameter\n");
//         exit(-1);
//     }
//     char *filename = argv[1];
//     FILE *read_file_ptr = fopen(filename,"rb");
//     struct stat info;
//     if (stat(filename, &info) != 0) {
//         printf("stat error \n");
//         exit(-1);
//     }
//     fread(buffer, (unsigned long)info.st_size, 1, read_file_ptr);

//     handle_ops((BaseRequest *)buffer);

// }