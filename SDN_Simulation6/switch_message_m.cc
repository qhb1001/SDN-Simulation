//
// Generated file, do not edit! Created by nedtool 5.4 from switch_message.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "switch_message_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(switch_message)

switch_message::switch_message(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->source = 0;
    this->destination = 0;
    for (unsigned int i=0; i<20; i++)
        this->loss[i] = 0;
    for (unsigned int i=0; i<20; i++)
        this->transmissionDelay[i] = 0;
    for (unsigned int i=0; i<20; i++)
        this->queuingDelay[i] = 0;
    for (unsigned int i=0; i<20; i++)
        this->availableBandwidth[i] = 0;
    for (unsigned int i=0; i<20; i++)
        this->totalBandwidth[i] = 0;
    this->hopCount = 0;
}

switch_message::switch_message(const switch_message& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

switch_message::~switch_message()
{
}

switch_message& switch_message::operator=(const switch_message& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void switch_message::copy(const switch_message& other)
{
    this->source = other.source;
    this->destination = other.destination;
    for (unsigned int i=0; i<20; i++)
        this->loss[i] = other.loss[i];
    for (unsigned int i=0; i<20; i++)
        this->transmissionDelay[i] = other.transmissionDelay[i];
    for (unsigned int i=0; i<20; i++)
        this->queuingDelay[i] = other.queuingDelay[i];
    for (unsigned int i=0; i<20; i++)
        this->availableBandwidth[i] = other.availableBandwidth[i];
    for (unsigned int i=0; i<20; i++)
        this->totalBandwidth[i] = other.totalBandwidth[i];
    this->hopCount = other.hopCount;
}

void switch_message::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->source);
    doParsimPacking(b,this->destination);
    doParsimArrayPacking(b,this->loss,20);
    doParsimArrayPacking(b,this->transmissionDelay,20);
    doParsimArrayPacking(b,this->queuingDelay,20);
    doParsimArrayPacking(b,this->availableBandwidth,20);
    doParsimArrayPacking(b,this->totalBandwidth,20);
    doParsimPacking(b,this->hopCount);
}

void switch_message::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->source);
    doParsimUnpacking(b,this->destination);
    doParsimArrayUnpacking(b,this->loss,20);
    doParsimArrayUnpacking(b,this->transmissionDelay,20);
    doParsimArrayUnpacking(b,this->queuingDelay,20);
    doParsimArrayUnpacking(b,this->availableBandwidth,20);
    doParsimArrayUnpacking(b,this->totalBandwidth,20);
    doParsimUnpacking(b,this->hopCount);
}

int switch_message::getSource() const
{
    return this->source;
}

void switch_message::setSource(int source)
{
    this->source = source;
}

int switch_message::getDestination() const
{
    return this->destination;
}

void switch_message::setDestination(int destination)
{
    this->destination = destination;
}

unsigned int switch_message::getLossArraySize() const
{
    return 20;
}

double switch_message::getLoss(unsigned int k) const
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    return this->loss[k];
}

void switch_message::setLoss(unsigned int k, double loss)
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    this->loss[k] = loss;
}

unsigned int switch_message::getTransmissionDelayArraySize() const
{
    return 20;
}

double switch_message::getTransmissionDelay(unsigned int k) const
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    return this->transmissionDelay[k];
}

void switch_message::setTransmissionDelay(unsigned int k, double transmissionDelay)
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    this->transmissionDelay[k] = transmissionDelay;
}

unsigned int switch_message::getQueuingDelayArraySize() const
{
    return 20;
}

double switch_message::getQueuingDelay(unsigned int k) const
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    return this->queuingDelay[k];
}

void switch_message::setQueuingDelay(unsigned int k, double queuingDelay)
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    this->queuingDelay[k] = queuingDelay;
}

unsigned int switch_message::getAvailableBandwidthArraySize() const
{
    return 20;
}

double switch_message::getAvailableBandwidth(unsigned int k) const
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    return this->availableBandwidth[k];
}

void switch_message::setAvailableBandwidth(unsigned int k, double availableBandwidth)
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    this->availableBandwidth[k] = availableBandwidth;
}

unsigned int switch_message::getTotalBandwidthArraySize() const
{
    return 20;
}

double switch_message::getTotalBandwidth(unsigned int k) const
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    return this->totalBandwidth[k];
}

void switch_message::setTotalBandwidth(unsigned int k, double totalBandwidth)
{
    if (k>=20) throw omnetpp::cRuntimeError("Array of size 20 indexed by %lu", (unsigned long)k);
    this->totalBandwidth[k] = totalBandwidth;
}

int switch_message::getHopCount() const
{
    return this->hopCount;
}

void switch_message::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

class switch_messageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    switch_messageDescriptor();
    virtual ~switch_messageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(switch_messageDescriptor)

switch_messageDescriptor::switch_messageDescriptor() : omnetpp::cClassDescriptor("switch_message", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

switch_messageDescriptor::~switch_messageDescriptor()
{
    delete[] propertynames;
}

bool switch_messageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<switch_message *>(obj)!=nullptr;
}

const char **switch_messageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *switch_messageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int switch_messageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount() : 8;
}

unsigned int switch_messageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<8) ? fieldTypeFlags[field] : 0;
}

const char *switch_messageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "source",
        "destination",
        "loss",
        "transmissionDelay",
        "queuingDelay",
        "availableBandwidth",
        "totalBandwidth",
        "hopCount",
    };
    return (field>=0 && field<8) ? fieldNames[field] : nullptr;
}

int switch_messageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "source")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destination")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "loss")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "transmissionDelay")==0) return base+3;
    if (fieldName[0]=='q' && strcmp(fieldName, "queuingDelay")==0) return base+4;
    if (fieldName[0]=='a' && strcmp(fieldName, "availableBandwidth")==0) return base+5;
    if (fieldName[0]=='t' && strcmp(fieldName, "totalBandwidth")==0) return base+6;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCount")==0) return base+7;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *switch_messageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "double",
        "double",
        "double",
        "double",
        "double",
        "int",
    };
    return (field>=0 && field<8) ? fieldTypeStrings[field] : nullptr;
}

const char **switch_messageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *switch_messageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int switch_messageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    switch_message *pp = (switch_message *)object; (void)pp;
    switch (field) {
        case 2: return 20;
        case 3: return 20;
        case 4: return 20;
        case 5: return 20;
        case 6: return 20;
        default: return 0;
    }
}

const char *switch_messageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    switch_message *pp = (switch_message *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string switch_messageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    switch_message *pp = (switch_message *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSource());
        case 1: return long2string(pp->getDestination());
        case 2: return double2string(pp->getLoss(i));
        case 3: return double2string(pp->getTransmissionDelay(i));
        case 4: return double2string(pp->getQueuingDelay(i));
        case 5: return double2string(pp->getAvailableBandwidth(i));
        case 6: return double2string(pp->getTotalBandwidth(i));
        case 7: return long2string(pp->getHopCount());
        default: return "";
    }
}

bool switch_messageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    switch_message *pp = (switch_message *)object; (void)pp;
    switch (field) {
        case 0: pp->setSource(string2long(value)); return true;
        case 1: pp->setDestination(string2long(value)); return true;
        case 2: pp->setLoss(i,string2double(value)); return true;
        case 3: pp->setTransmissionDelay(i,string2double(value)); return true;
        case 4: pp->setQueuingDelay(i,string2double(value)); return true;
        case 5: pp->setAvailableBandwidth(i,string2double(value)); return true;
        case 6: pp->setTotalBandwidth(i,string2double(value)); return true;
        case 7: pp->setHopCount(string2long(value)); return true;
        default: return false;
    }
}

const char *switch_messageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *switch_messageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    switch_message *pp = (switch_message *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


