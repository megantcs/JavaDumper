//
// Created by inter on 08.11.2025.
//

#include "JNIDumperStruct.h"

#include <regex>
#include <set>

JNIDumperStruct::JNIDumperStruct(const JNIContainer &container)
    : jni(container)
{

}

std::string ExtractClassPath(const std::string &sig) {
    if (sig.size() > 2 && sig[0] == '[' && sig[1] == 'L' && sig.back() == ';') {
        return sig.substr(2, sig.length() - 3);
    }
    if (sig.size() > 2 && sig[0] == 'L' && sig.back() == ';') {
        return sig.substr(1, sig.length() - 2);
    }
    return sig;
}

bool IsPackagePattern(const std::string &signature, std::vector<std::string> patterns) {
    std::string className = ExtractClassPath(signature);

    for (const auto &pattern : patterns) {
        if (className.find(pattern) == 0) return true;
    }
    return false;
}

bool IsJdkSignature(const std::string& signature) {
    std::string clean_signature = signature;

    std::string className = ExtractClassPath(clean_signature);

    if (className.find("java/") == 0 ||
        className.find("jdk/") == 0 ||
        className.find("sun/") == 0 ||
        className.find("com/sun/") == 0 ||
        className.find("javax/") == 0 ||
        className.find("jdk/internal/") == 0 ||
        className.find("sunw/") == 0) {
        return true;
        }

    if (signature.size() >= 2 && signature[0] == '[') {
        char type = signature[1];
        if (type == 'I' || type == 'J' || type == 'S' || type == 'B' ||
            type == 'D' || type == 'F' || type == 'C' || type == 'Z') {
            return true;
            }
    }

    if (signature.find("[[") == 0) {
        return true;
    }

    return false;
}
bool IsLambdaSignature(const std::string& signature) {
    std::string className = signature;

    if (className.size() > 2 && className[0] == 'L' && className.back() == ';') {
        className = className.substr(1, className.length() - 2);
    }

    return (className.find("$$Lambda$") != std::string::npos) ||
           (className.find("/Lambda$") != std::string::npos) ||
           (className.find("$Lambda") != std::string::npos &&
            className.find("0x") != std::string::npos);
}

std::vector<JNIDumperStruct::ClassInfo> JNIDumperStruct::GetAllClassInfo(bool dumpMethods, bool dumpFields, bool jdk_ignore, bool lambda_ignore, const std::vector<std::string> &ignore_package)
{
    std::vector<JNIDumperStruct::ClassInfo> buff;
    jclass* classes;
    jint count;

    jni.jvmti->GetLoadedClasses(&count, &classes);

    for (jint i = 0; i < count; i++)
    {
        char* signature_ptr;
        char* generic_ptr;

        jclass klass = classes[i];
        jni.jvmti->GetClassSignature(klass, &signature_ptr, &generic_ptr);

        std::string signature = signature_ptr;
        std::string generic = generic_ptr ? generic_ptr : "";

        jni.jvmti->Deallocate((unsigned char*)signature_ptr);
        jni.jvmti->Deallocate((unsigned char*)generic_ptr);


        if (jdk_ignore) if (IsJdkSignature(signature)) continue;
        if (lambda_ignore) if (IsLambdaSignature(signature)) continue;
        if (IsPackagePattern(signature, ignore_package))
        {
            continue;
        }
        jint fields_count;
        jfieldID* fields;
        jni.jvmti->GetClassFields(klass, &fields_count, &fields);
        std::vector<FieldInfo> fieldsInfo;

        if (dumpFields) {
            for (int j = 0; j < fields_count; j++)
            {
                FieldInfo fieldInfo;
                fieldInfo.field_ptr = fields[j];

                char* name_ptr = nullptr;
                char* signature_ptr = nullptr;
                char* generic_ptr = nullptr;

                jni.jvmti->GetFieldName(klass, fields[j], &name_ptr, &signature_ptr, &generic_ptr);
                fieldInfo.name = name_ptr ? name_ptr : "null";
                fieldInfo.signature = signature_ptr ? signature_ptr : "null";
                fieldInfo.generic = generic_ptr ? generic_ptr : "null";

                jni.jvmti->GetFieldModifiers(klass, fields[j], &fieldInfo.modifiers);

                if (name_ptr) jni.jvmti->Deallocate((unsigned char*)name_ptr);
                if (signature_ptr) jni.jvmti->Deallocate((unsigned char*)signature_ptr);
                if (generic_ptr) jni.jvmti->Deallocate((unsigned char*)generic_ptr);

                fieldsInfo.push_back(fieldInfo);
            }
        }
        std::vector<MethodInfo> methodsInfo;
        if (dumpMethods) {
            jint methods_count;
            jmethodID* methods;
            jni.jvmti->GetClassMethods(klass, &methods_count, &methods);


            for (int j = 0; j < methods_count; j++)
            {
                MethodInfo methodInfo;
                methodInfo.method_ptr = methods[j];

                char* name_ptr = nullptr;
                char* signature_ptr = nullptr;
                char* generic_ptr = nullptr;

                jni.jvmti->GetMethodName(methods[j], &name_ptr, &signature_ptr, &generic_ptr);
                methodInfo.name = name_ptr ? name_ptr : "null";
                methodInfo.signature = signature_ptr ? signature_ptr : "null";
                methodInfo.generic = generic_ptr ? generic_ptr : "null";

                jni.jvmti->GetMethodModifiers(methods[j], &methodInfo.modifiers);

                if (name_ptr) jni.jvmti->Deallocate((unsigned char*)name_ptr);
                if (signature_ptr) jni.jvmti->Deallocate((unsigned char*)signature_ptr);
                if (generic_ptr) jni.jvmti->Deallocate((unsigned char*)generic_ptr);

                methodsInfo.push_back(methodInfo);
            }
        }

        ClassInfo info;

        info.signature = signature;
        if (generic.empty()) {
            info.generic = ""; // шел нахуй optional
        }
        else {
            info.generic = generic_ptr;
        }

        info.class_ptr = (jclass)jni.env->NewLocalRef(klass);
        info.methods = methodsInfo;
        info.fields = fieldsInfo;

        buff.push_back(info);
    }

    return buff;
}
