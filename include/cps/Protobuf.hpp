#ifndef CPS_PROTOBUF_HPP
#define CPS_PROTOBUF_HPP

#include <string>
#include <boost/lexical_cast.hpp>

namespace CPS
{
enum {
    ProtobufWireType_Varint,
    ProtobufWireType_64bit,
    ProtobufWireType_LengthDelimited,
    ProtobufWireType_StartGroup,
    ProtobufWireType_EndGroup,
    ProtobufWireType_32bit
};

static std::string varintToBytes(unsigned int value)
{
    std::string result = "";
    char byte = 0x00;
    while (value > 0) {
        // Copy first 7 bits
        byte = value & 0x7F;
        // Shift to process next bits
        value = value >> 7;
        // Set first bit to indicate more numbers are to follow
        if (value > 0)
            byte |= 0x80;
        // Add byte to result
        result.push_back(byte);
    }
    return result;
}

static unsigned int bytesToVarint(std::vector<unsigned char> &bytes,
                                  unsigned int offset, unsigned int &result)
{
    unsigned int shift = 0;
    unsigned int parsed = 0;
    for (; offset + parsed < bytes.size(); parsed++) {
        if (shift > 31) { // Should actually check if shift is < 32, to avoid overflow
            BOOST_THROW_EXCEPTION(CPS::Exception("Integer overflow", 9005));
        }
        result += (bytes[offset + parsed] & 0x7F) << shift;
        shift += 7;
        if ((bytes[offset + parsed] & 0x80) == 0)
            return parsed + 1;
    }
    return parsed;
}

class ProtobufField
{
public:
    ProtobufField(): fieldNumber(0), wireType(0) {
    }

    virtual ~ProtobufField() {
    }

    int fromBytes(std::vector<unsigned char> &stream, unsigned int &offset) {
        unsigned int value = 0;
        unsigned int parsed = bytesToVarint(stream, offset, value);
        if (parsed == 0)
            return parsed;
        unsigned int originalOffset = offset;
        offset += parsed;

        this->wireType = (value & 0x07);
        this->fieldNumber = (value >> 3);
        value = 0;

        if (this->wireType == ProtobufWireType_Varint) {
            parsed = bytesToVarint(stream, offset, value);
            offset += parsed + value;
            this->data = boost::lexical_cast<std::string>(value);
        } else if (this->wireType == ProtobufWireType_LengthDelimited) {
            parsed = bytesToVarint(stream, offset, value);
            if (parsed == 0)
                return parsed;
            this->data = std::string(stream.begin() + offset + parsed,
                                     stream.begin() + offset + parsed + value);
            offset += parsed + value;
        } else if (this->wireType == ProtobufWireType_32bit) {
            this->data = std::string(stream.begin() + offset, stream.begin() + offset + 4);
            offset += 4;
        } else {
            BOOST_THROW_EXCEPTION(CPS::Exception("Not supported protocol buffer wire type", 9005));
        }
        return offset - originalOffset;
    }

    std::string toString() {
        std::string res = "";
        res.push_back((this->fieldNumber << 3) | this->wireType);
        if (this->wireType == ProtobufWireType_LengthDelimited) {
            res += varintToBytes(this->data.size());
        }
        res += this->data;
        return res;
    }

    unsigned int fieldNumber;
    unsigned int wireType;
    std::string data;
};

class Protobuf
{
public:
    Protobuf() {

    }
    virtual ~Protobuf() {
        destroyFields();
    }

    ProtobufField* getField(unsigned int fieldNumber) {
        for (unsigned int i = 0; i < this->fields.size(); i++) {
            if (this->fields[i]->fieldNumber == fieldNumber) {
                return this->fields[i];
            }
        }
        return NULL;
    }

    void destroyFields() {
        for (unsigned int i = 0; i < this->fields.size(); i++) {
            delete this->fields[i];
        }
        this->fields.clear();
    }

    void newField(unsigned int fieldNumber, unsigned int wireType,
                  const std::string &data) {
        ProtobufField * field = new ProtobufField();
        field->fieldNumber = fieldNumber;
        field->wireType = wireType;
        field->data = data;
        this->fields.push_back(field);
    }

    void newFieldString(unsigned int fieldNumber, const std::string &data) {
        newField(fieldNumber, ProtobufWireType_LengthDelimited, data);
    }

    void newFieldBool(unsigned int fieldNumber, bool value) {
        newField(fieldNumber, ProtobufWireType_Varint, varintToBytes(value));
    }

    void fromBytes(std::vector<unsigned char> &stream) {
        destroyFields();
        for (unsigned int i = 0; i < stream.size();) {
            ProtobufField * field = new ProtobufField();
            if (field->fromBytes(stream, i) > 0) {
                this->fields.push_back(field);
            } else {
                delete field;
            }
        }
    }

    std::string toString() {
        std::string res = "";
        for (unsigned int i = 0; i < this->fields.size(); i++) {
            res += this->fields[i]->toString();
        }
        return res;
    }

public:
    std::vector<ProtobufField*> fields;
};
}

#endif //#ifndef CPS_RESPONSE_HPP 
