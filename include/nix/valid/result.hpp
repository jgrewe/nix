// Copyright (c) 2014, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#ifndef NIX_RESULT_H
#define NIX_RESULT_H

#include <nix/Platform.hpp>
#include <nix/None.hpp>
#include <nix/valid/helper.hpp>

#include <vector>
#include <string>
#include <ostream>

namespace nix {
namespace valid {

class NIXAPI Result {
    std::vector<Message> errors;
    std::vector<Message> warnings;
public:

    /**
     * @brief Standard ctor.
     * 
     * Standard ctor that expects a vector of error msgs and a vector
     * of warning msgs.
     *
     * @param errs Vector of error messages
     * @param warns Vector of warning messages
     */
    Result(const std::vector<Message> &errs = std::vector<Message>(),
           const std::vector<Message> &warns = std::vector<Message>());

    /**
     * @brief Only warnings ctor.
     * 
     * Only warnings ctor that takes boost::none as 1st parameter and
     * expects a vector of warning msgs as 2nd parameter
     *
     * @param t boost::none
     * @param warns Vector of warning messages
     */
    Result(none_t t, const std::vector<Message> &warns);

    /**
     * @brief Only errors ctor.
     * 
     * Only errors ctor that expects a vector of error msgs as 1st
     * parameter and takes boost::none as 2nd parameter
     *
     * @param errs Vector of warning messages
     * @param t boost::none
     */
    Result(const std::vector<Message> & errs, none_t t);

    /**
     * @brief Non-vector only warnings ctor.
     * 
     * Non-vector only warnings ctor that takes boost::none as 1st
     * parameter and expects a warning msg as 2nd parameter
     *
     * @param t boost::none
     * @param warn Vector of warning messages
     */
    Result(none_t t, const Message &warn);

    /**
     * @brief Non-vector only errors ctor.
     * 
     * Non-vector only errors ctor that expects an error msg as 1st
     * parameter an takes boost::none as 2nd parameter
     *
     * @param err Vector of warning messages
     * @param t boost::none
     */
    Result(const Message &err, none_t t);

    /**
     * @brief Returns the warnings vector.
     * 
     * Returns the {@link warnings} vector
     *
     * @return vector of warning msgs
     */
    std::vector<Message> getWarnings() const;

    /**
     * @brief Returns the errors vector.
     * 
     * Returns the {@link errors} vector
     *
     * @return vector of error msgs
     */
    std::vector<Message> getErrors() const;

    /**
     * @brief Appends the warnings & errors of given Result to this one
     * 
     * Concatenates the {@link errors} and {@link warnings} vectors
     * of the given {@link Result} object to those if this {@link Result}
     * object and returns a reference to this object.
     *
     * @return reference to this Result
     */
    Result concat(const Result &result);

    /**
     * @brief Adds an error message
     * 
     * Adds an error message to this {@link Result}
     * object and returns a reference to this object.
     *
     * @return reference to this Result
     */
    Result addError(const Message &error);

    /**
     * @brief Adds a warning message
     * 
     * Adds a warning message to this {@link Result}
     * object and returns a reference to this object.
     *
     * @return reference to this Result
     */
    Result addWarning(const Message &warning);

    /**
     * @brief Returns true if no msgs added at all
     * 
     * Returns true if neither errors nor warnings have been added,
     * thus both vectors are empty. Returns false otherwise.
     *
     * @return bool indicating whether no msgs added at all
     */
    bool ok() const;

    /**
     * @brief Returns true if no error msgs added at all
     * 
     * Returns false if no errors have been added, thus vector is empty.
     * Returns true otherwise.
     *
     * @return bool indicating whether error msgs added
     */
    bool hasErrors() const;

    /**
     * @brief Returns true if no warning msgs added at all
     * 
     * Returns false if no warnings have been added, thus vector is empty.
     * Returns true otherwise.
     *
     * @return bool indicating whether warning msgs added
     */
    bool hasWarnings() const;

    /**
     * @brief Output operator
     * 
     * Output operator to output all msgs in the Result to the given
     * stream
     *
     * @return std::ostream& reference
     */
    NIXAPI friend std::ostream& operator<<(std::ostream &out, const Result &res) {
        // make temp copies to set prefixes on
        std::vector<Message> tmp_errors = res.getErrors();
        std::vector<Message> tmp_warnings = res.getWarnings();

        for (const auto &warn : tmp_warnings) {
            if (!warn.id.empty())
                out << "ID " << warn.id << " ";
            out << "WARNING: " << warn.msg << std::endl;
        }

        for (const auto &err : tmp_errors) {
            if (!err.id.empty())
                out << "ID " << err.id << " ";

            out << "ERROR: " << err.msg << std::endl;
        }

        return out;
    }
};

} // namespace valid
} // namespace nix

#endif // NIX_RESULT_H
