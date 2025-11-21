//
// Created by inter on 08.11.2025.
//

#ifndef UNTITLED2_DUMPERSTRUCT_H
#define UNTITLED2_DUMPERSTRUCT_H

#include <optional>
#include <vector>

#include "basic.h"

struct Settings {
    bool show_lambda_expressions = true;
    bool show_jdk = true;
};

struct JNIDumperStruct
{
    struct MethodInfo {
        std::string name;
        std::string signature;
        std::string generic;
        jmethodID method_ptr;
        jint modifiers;
    };

    struct FieldInfo {
        std::string name;
        std::string signature;
        std::string generic;
        jfieldID field_ptr;
        jint modifiers;
    };

    struct ClassInfo {
        std::string signature;
        jclass class_ptr;
        std::string generic; // какой нахуй optinal вы че ебанулись

        std::vector<MethodInfo> methods;
        std::vector<FieldInfo> fields;
    };
private:
    JNIContainer jni;
public:
    explicit JNIDumperStruct(const JNIContainer &container);
    std::vector<ClassInfo> GetAllClassInfo(bool dumpMethods, bool dumpFields,bool jdk_ignore, bool lambda_ignore, const std::vector<std::string> &ignore_package);

    Settings settings;
};


#endif //UNTITLED2_DUMPERSTRUCT_H