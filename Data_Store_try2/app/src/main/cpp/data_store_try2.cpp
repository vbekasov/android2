#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>
#include "sqlite3.h"
#include <sstream>
// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("data_store_try2");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("data_store_try2")
//      }
//    }
extern "C" JNIEXPORT jint JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1retstr(JNIEnv *env, jobject thiz) {
    int ret_int = 5;
    
    return ret_int;
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1strfun(JNIEnv *env, jobject thiz,
 jstring txt) {
    jstring S1;
    char msg[60] = "Test text";
    const char *input = env ->GetStringUTFChars(txt, 0);
    std::string full_pth(input);
    full_pth += "/cpptest.txt";
    S1 = env ->NewStringUTF(input);
    std::ofstream nFile(full_pth);
    nFile << "New File line\n";
    nFile.close();
    return S1;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1readtxt(JNIEnv *env, jobject thiz,
                                                               jstring pth) {
    const char *input = env ->GetStringUTFChars(pth, 0);
    std::string full_pth(input);
    full_pth += "/cpptest.txt";
    std::ifstream fTxt(full_pth);
    std::string f_line;
    std::string f_content = "";

    while (getline(fTxt, f_line)){f_content += f_line;}
    fTxt.close();

    const char *output = f_content.c_str();
    jstring out;
    out = env ->NewStringUTF(output);

    return out;
}
int callback_emptyfun(void *NotUsed, int argc, char **argv, char **azColName){return 0;} // sqlite support fun
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1generateDB(JNIEnv *env, jobject thiz,
                                                                  jstring pth) {
    const char *input = env ->GetStringUTFChars(pth, 0);
    std::string full_pth(input);
    full_pth += "/cpptest.db";
    jstring out;

    sqlite3 *db;
    char  *zErrMsg = 0;
    int rc;
    std::string ret_str;
    std::string sql_command = "";

    rc = sqlite3_open(full_pth.c_str(), &db);
    if (rc){
        ret_str = "Can`t open\n";
        out = env ->NewStringUTF(ret_str.c_str());
        return out;
    } else {
        ret_str = "Opened db successfully\n";
    }

    sql_command = "CREATE TABLE IF NOT EXISTS test_table " \
                  "(id INTEGER PRIMARY KEY AUTOINCREMENT, " \
                  "name TEXT);";

    rc = sqlite3_exec(db, sql_command.c_str(),callback_emptyfun, 0, &zErrMsg);
    if (rc != SQLITE_OK){
        ret_str += "Table err\n";
    } else {
        ret_str += "Table ok";
    }

    sqlite3_close(db);

    out = env ->NewStringUTF(ret_str.c_str());
    return out;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1appendDB(JNIEnv *env, jobject thiz,
                                                                jstring pth, jstring text_name) {
    std::string full_pth(env ->GetStringUTFChars(pth, 0)); full_pth += "/cpptest.db";
    std::string u_name(env ->GetStringUTFChars(text_name, 0));
    jstring out;
    std::string ret_str = "";

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::string sql_command;

    rc = sqlite3_open(full_pth.c_str(), &db);

    if (rc){
        ret_str += "Can`t open DB\n";
        out = env -> NewStringUTF(ret_str.c_str());
        return out;
    } else {
        ret_str += "Open OK\n";
    }

    sql_command = "INSERT INTO test_table (name) VALUES ( '" + u_name + "');";
    rc = sqlite3_exec(db, sql_command.c_str(), callback_emptyfun, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        ret_str += "!!!NOK. Input error: ";
        ret_str += zErrMsg;
    } else {
        ret_str += "OK input";
    }
    sqlite3_close(db);

    out = env -> NewStringUTF(ret_str.c_str());
    return out;
}

class data_out{
public: char* data;
};
int callback_select(void *data, int argc, char **argv, char **azColName){
    int i;
    //fprintf(stderr, "%s: ", (const char*)data);

    data = (void *)'C';

    for(i = 0; i<argc; i++){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    //printf("\n");
    return 0;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_data_1store_1try2_DB_1SQLite_1try2_c_1readDB(JNIEnv *env, jobject thiz,
                                                              jstring pth) {
    std::string full_pth(env ->GetStringUTFChars(pth, 0)); full_pth += "/cpptest.db";
    std::string sql_command;
    jstring out;
    int id;
    std::string uname;
    std::string ret_str = "";

    sqlite3 *db;
    char *zErrMrg = 0;
    int rc;
    char *sql;
    char* data;
    sqlite3_stmt *sql_stmt;

    rc = sqlite3_open(full_pth.c_str(), &db);

    sql_command = "SELECT * FROM test_table;";

    //rc = sqlite3_exec(db, sql_command.c_str(),callback_select,(void *)data, &zErrMrg);
    sqlite3_prepare_v2(db, sql_command.c_str(), -1, &sql_stmt, NULL);
    sqlite3_bind_int(sql_stmt, 1, 2);
    while (sqlite3_step(sql_stmt) != SQLITE_DONE){
        id = sqlite3_column_int(sql_stmt, 0);
        uname = reinterpret_cast<const char *>(sqlite3_column_text(sql_stmt, 1));
        ret_str += "id : " + std::to_string(id) + "\nName : " + uname + "\n";
    }
    sqlite3_finalize(sql_stmt);
    sqlite3_close(db);
    out = env ->NewStringUTF(ret_str.c_str());
    return out;
}









