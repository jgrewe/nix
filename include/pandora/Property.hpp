#ifndef PAN_PROPERTY_H_INCLUDE
#define PAN_PROPERTY_H_INCLUDE

#include <string>
#include <H5Cpp.h>

#include <pandora/Group.hpp>
#include <pandora/Section.hpp>
#include <pandora/Value.hpp>

#include <pandora/DataSet.hpp>

namespace pandora {

class Property {

private:

  Section section;
  Group group;
  std::string property_id;

  bool checkDataType(const H5::DataSet &dataset, H5T_class_t type) const;

public:

  Property(const Property &property);

  Property(Section section, Group group, std::string id);

  std::string id() const;

  void name(std::string name);
  std::string name() const;

  void definition(std::string definition);
  std::string definition() const;

  void link(std::string link);
  std::string link() const;

  void include(std::string include);
  std::string include() const;

  void mapping(std::string mapping);
  std::string mapping() const;

  void dataType(std::string dataType);
  std::string dataType() const;

  void unit(std::string unit);
  std::string unit() const;

  template<typename T>
  void addValue(const Value<T> &value);

  template<typename T>
  void addValue(const T value, double uncertainty = 0.0, const std::string &reference = "", const std::string filename = "",
      const std::string encoder = "", const std::string checksum = "");


  template<typename T>
  void value(size_t index, Value<T> &value) const;

  void removeValue(size_t index);

  void removeValues();

  size_t valueCount() const;

  //void addValue(int value, const std::string &reference, const std::string &filename, const std::string filename, const std::string encoder, const std::string checksum);


  bool operator==(const Property &other) const;

  bool operator!=(const Property &other) const;

  virtual ~Property();

};

template<typename T>
void Property::addValue(T value, double uncertainty, const std::string &reference, const std::string filename,
    const std::string encoder, const std::string checksum) {

  std::string dt = this->dataType();
//  if (dt.length() > 0 && dt.compare(info.type) != 0) {
//    throw std::runtime_error("Value and data type do not match!");
//    return;
//  } else {
//    dataType(info.type);
//  }
  Value<T> tempValue(value, uncertainty, reference, filename, encoder, checksum);
  addValue(tempValue);
}

template<typename T>
void Property::addValue(const Value<T> &value) {
  std::vector<Value<T> > vals = { value };

  PSize start;

  DataSet ds((H5::DataSet()));
  if (group.hasData("values")) {
    ds = group.openData("values");
    PSize size = ds.size();
    PSize newSize = size + 1;
    ds.extend(newSize);
    start = size;
  } else {
    Charon<std::vector<Value<T> > > charon(vals);
    PSize size = {1};
    PSize maxsize = {H5S_UNLIMITED};
    PSize chunks = DataSet::guessChunking(size, DataType::Double);
    ds = DataSet::create(group.h5Group(), charon.getFileType(),  "values", size, &maxsize, &chunks);
    start = {0};
  }

  Selection fileSel = ds.createSelection();
  PSize count = {1};
  fileSel.select(count, start);

  ds.write(vals, fileSel);
}

template<typename T>
void Property::value(size_t index, Value<T> &value) const {
  if (group.hasData("values")) {
    if (index < 0 || index >= valueCount()) {
      throw std::runtime_error("Property::stringValue(index): Index out of bounds!");
    }
    DataSet dataset = group.openData("values");

//    ValueInfo<T> info;
//    if (!checkDataType(dataset, info.h5class)) {
//      throw std::runtime_error("Property::stringValue(index): Value DataType is not String!");
//    }

    std::vector<Value<T> > vals;

    Selection fileSel = dataset.createSelection();
    PSize start = {index};
    PSize count = {1};
    fileSel.select(count, start);

    dataset.read(vals, fileSel, true);
    value = vals[0];
  }
}
}

#endif // HDX_PROPERTY_H_INCLUDE
