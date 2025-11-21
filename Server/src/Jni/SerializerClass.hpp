#pragma once

#include "JNIDumperStruct.h"

typedef JNIDumperStruct::ClassInfo IClass;
typedef JNIDumperStruct::FieldInfo IField;
typedef JNIDumperStruct::MethodInfo IMethod;

std::string GetHeaderClassses(const std::vector<IClass> &classes) 
{
	std::string buf;
	for (const auto& klass : classes) {
		buf += klass.signature;

		for (const auto& field : klass.fields)
			buf += "  -- field " + field.signature + " " + field.name;
		for (const auto& method : klass.methods)
			buf += "  -- method " + method.name + ":" + method.signature;
	
		buf += "\n";
	}

	return buf;
}