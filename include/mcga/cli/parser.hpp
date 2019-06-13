#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "argument.hpp"
#include "choice_argument.hpp"
#include "command_line_spec.hpp"
#include "flag.hpp"
#include "numeric_argument.hpp"

namespace mcga::cli {

class Parser {
  public:
    using ArgList = std::vector<std::string>;

    explicit Parser(const std::string& _helpPrefix)
            : helpPrefix(_helpPrefix + "\n") {
    }

    MCGA_DISALLOW_COPY_AND_MOVE(Parser);

    ~Parser() = default;

    Argument addArgument(const ArgumentSpec& spec) {
        checkNameAvailability(spec.name, spec.shortName);
        Argument argument
          = std::make_shared<internal::Argument::MakeSharedEnabler>(spec);
        addSpec(argument, spec.name, spec.shortName);
        std::string extra;
        if (spec.defaultValue.has_value() && spec.implicitValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription() + "', Implicit: '"
              + spec.implicitValue.value().getDescription() + "'";
        } else if (spec.defaultValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription() + "'";
        } else if (spec.implicitValue.has_value()) {
            extra = "\t\tImplicit: '"
              + spec.implicitValue.value().getDescription() + "'";
        }
        addHelp(
          spec.helpGroup, spec.name, spec.shortName, spec.description, extra);
        return argument;
    }

    template<class T>
    NumericArgument<T> addNumericArgument(const NumericArgumentSpec<T>& spec) {
        checkNameAvailability(spec.name, spec.shortName);
        NumericArgument<T> argument = std::make_shared<
          typename internal::NumericArgument<T>::MakeSharedEnabler>(spec);
        addSpec(argument, spec.name, spec.shortName);
        std::string extra;
        if (spec.defaultValue.has_value() && spec.implicitValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription() + "', Implicit: '"
              + spec.implicitValue.value().getDescription() + "'";
        } else if (spec.defaultValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription() + "'";
        } else if (spec.implicitValue.has_value()) {
            extra = "\t\tImplicit: '"
              + spec.implicitValue.value().getDescription() + "'";
        }
        addHelp(spec.helpGroup,
                spec.name,
                spec.shortName,
                "(Number)\t" + spec.description,
                extra);
        return argument;
    }

    Flag addFlag(const FlagSpec& spec) {
        checkNameAvailability(spec.name, spec.shortName);
        Flag flag
          = std::make_shared<internal::Flag::FlagMakeSharedEnabler>(spec);
        addSpec(flag, spec.name, spec.shortName);
        addHelp(spec.helpGroup,
                spec.name,
                spec.shortName,
                "(Flag)\t" + spec.description,
                "");
        return flag;
    }

    void addTerminalFlag(const FlagSpec& spec,
                         const std::function<void()>& callback) {
        terminalFlags.emplace_back(addFlag(spec), callback);
    }

    void addTerminalFlag(const FlagSpec& spec, const std::string& message) {
        terminalFlags.emplace_back(addFlag(spec),
                                   [message] { std::cout << message; });
    }

    void addHelpFlag() {
        addTerminalFlag(FlagSpec("help").setShortName("h").setDescription(
                          "Display this help menu."),
                        [this]() { std::cout << renderHelp(); });
    }

    template<class T>
    ChoiceArgument<T> addChoiceArgument(const ChoiceArgumentSpec<T>& spec) {
        checkNameAvailability(spec.name, spec.shortName);

        std::string renderedOptions;
        bool first = true;
        for (const std::pair<std::string, T>& option: spec.options) {
            if (!first) {
                renderedOptions += ",";
            }
            first = false;
            renderedOptions += "'" + option.first + "'";
        }
        renderedOptions = "[" + renderedOptions + "]";
        ChoiceArgument<T> arg = std::make_shared<
          typename internal::ChoiceArgument<T>::MakeSharedEnabler>(spec);
        addSpec(arg, spec.name, spec.shortName);
        std::string extra;
        if (spec.defaultValue.has_value() && spec.implicitValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription() + "', Implicit: '"
              + spec.implicitValue.value().getDescription()
              + "', allowed values: " + renderedOptions;
        } else if (spec.defaultValue.has_value()) {
            extra = "\t\tDefault: '"
              + spec.defaultValue.value().getDescription()
              + "', allowed values: " + renderedOptions;
        } else if (spec.implicitValue.has_value()) {
            extra = "\t\tImplicit: '"
              + spec.implicitValue.value().getDescription()
              + "', allowed values: " + renderedOptions;
        }

        addHelp(
          spec.helpGroup, spec.name, spec.shortName, spec.description, extra);
        return arg;
    }

    ArgList parse(const ArgList& args) {
        for (const CommandLineSpecPtr& spec: specs) {
            spec->appearedInArgs = false;
        }

        ArgList positionalArguments;
        std::string lastShortName;
        bool onlyPositional = false;
        for (const std::string& arg: args) {
            // on encountering the "--" argument, all arguments from that point
            // on are considered positional.
            if (arg == "--") {
                onlyPositional = true;
                continue;
            }

            // all arguments after "--" are considered positional.
            if (onlyPositional) {
                positionalArguments.push_back(arg);
                continue;
            }

            // an argument that does not start with '-' (or is equal to "-") is
            // not considered special, and therefore will be treated like either
            // a positional argument or a value filler for the last unfulfilled
            // short name argument given in the format "-XYZ".
            if (arg.substr(0, 1) != "-" || arg == "-") {
                if (shouldApplyValue(lastShortName)) {
                    // `lastShortName` is an unfulfilled argument given by short
                    // name in the format "-XYZ v" as "Z".
                    applyValue(lastShortName, arg);
                    lastShortName = "";
                } else {
                    // no unfulfilled argument given by short name, considering
                    // a positional argument.
                    positionalArguments.push_back(arg);
                }
                continue;
            }

            // if we reached this point without hitting a `continue`, now the
            // current argument is definitely a special one.

            // if we had an unfulfilled argument given by short name, we will
            // give it its implicit value since it won't be fulfilled by this
            // argument.
            if (!lastShortName.empty()) {
                applyImplicit(lastShortName);
                lastShortName = "";
            }

            auto equalPos = arg.find('=');

            // 1. for "--X", give argument "X" its implicit value
            if (arg.substr(0, 2) == "--" && equalPos == std::string::npos) {
                applyImplicit(arg.substr(2));
            }

            // 2. for "--X=v", give argument "X" value "v"
            if (arg.substr(0, 2) == "--" && equalPos != std::string::npos) {
                applyValue(arg.substr(2, equalPos - 2),
                           arg.substr(equalPos + 1));
            }

            // 3. for "-XYZ", give arguments "X" and "Y" their implicit values,
            // and remember "Z" as the last short name, as a construct of the
            // form "-XYZ v" is allowed, equivalent with "--X --Y --Z=v".
            if (arg.substr(0, 2) != "--" && equalPos == std::string::npos) {
                for (size_t j = 1; j + 1 < arg.length(); ++j) {
                    applyImplicit(arg.substr(j, 1));
                }
                lastShortName = arg.substr(arg.length() - 1, 1);
            }

            // 4. for "-XYZ=v", give arguments "X" and "Y" their implicit values
            // and argument "Z" value "v".
            if (arg.substr(0, 2) != "--" && equalPos != std::string::npos) {
                for (size_t j = 1; j + 1 < equalPos; ++j) {
                    applyImplicit(arg.substr(j, 1));
                }
                applyValue(arg.substr(equalPos - 1, 1),
                           arg.substr(equalPos + 1));
            }
        }
        if (!lastShortName.empty()) {
            applyImplicit(lastShortName);
        }

        for (const CommandLineSpecPtr& spec: specs) {
            if (!spec->appeared()) {
                spec->setDefaultGuarded();
            }
        }

        for (const auto& flag: terminalFlags) {
            if (flag.first->getValue()) {
                flag.second();
                exit(0);
            }
        }

        return positionalArguments;
    }

    ArgList parse(int argc, char** argv) {
        ArgList args(static_cast<std::size_t>(argc));
        for (int i = 0; i < argc; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            args.emplace_back(argv[i]);
        }
        return parse(args);
    }

    std::string renderHelp() const {
        std::string help = helpPrefix + "\n";
        for (const HelpGroup& group: helpSections) {
            help += "\n" + group.content;
        }
        return help;
    }

  private:
    using CommandLineSpecPtr = std::shared_ptr<internal::CommandLineSpec>;

    struct HelpGroup {
        std::string groupName;
        std::string content;
    };

    void addSpec(const CommandLineSpecPtr& spec,
                 const std::string& name,
                 const std::string& shortName) {
        specs.push_back(spec);
        reservedNames.insert(name);
        specsByCliString[name] = spec;
        if (!shortName.empty()) {
            reservedNames.insert(shortName);
            specsByCliString[shortName] = spec;
        }
    }

    void applyValue(const std::string& cliString, const std::string& value) {
        auto specIterator = specsByCliString.find(cliString);
        if (specIterator != specsByCliString.end()) {
            specIterator->second->setValueGuarded(value);
        }
    }

    void applyImplicit(const std::string& cliString) {
        auto specIterator = specsByCliString.find(cliString);
        if (specIterator != specsByCliString.end()) {
            specIterator->second->setImplicitGuarded();
        }
    }

    bool shouldApplyValue(const std::string& cliString) const {
        auto specIterator = specsByCliString.find(cliString);
        return specIterator != specsByCliString.end()
          && specIterator->second->takesNextPositionalArg();
    }

    void checkNameAvailability(const std::string& name,
                               const std::string& shortName) const {
        if (reservedNames.count(name) != 0) {
            throw std::runtime_error(
              "Argument tried to register " + name
              + " as a command-line "
                "name, but a different argument already has it as a name.");
        }
        if (!shortName.empty() && reservedNames.count(shortName) != 0) {
            throw std::runtime_error(
              "Argument tried to register " + shortName
              + " as a command-line"
                " short name, but a different argument already has it as a "
                "short name.");
        }
        if (shortName.size() > 1) {
            throw std::runtime_error(
              "Argument short name should always have length 1.");
        }
    }

    void addHelp(const std::string& helpGroup,
                 const std::string& name,
                 const std::string& shortName,
                 const std::string& description,
                 const std::string& extra) {
        std::string helpLine = "\t--" + name;
        if (!shortName.empty()) {
            helpLine += ",-" + shortName;
        }
        helpLine += "\t" + description;
        if (!extra.empty()) {
            helpLine += "\n" + extra;
        }

        if (helpGroup.empty()) {
            helpPrefix += "\n" + helpLine;
            return;
        }

        bool foundHelpGroup = false;
        for (HelpGroup& group: helpSections) {
            if (group.groupName == helpGroup) {
                foundHelpGroup = true;
                group.content += helpLine + "\n";
                break;
            }
        }
        if (!foundHelpGroup) {
            helpSections.push_back(
              {helpGroup, helpGroup + "\n" + helpLine + "\n"});
        }
    }

    template<class T>
    static inline std::string toString(const T& value) {
        return std::to_string(value);
    }

    std::vector<CommandLineSpecPtr> specs;
    std::map<std::string, CommandLineSpecPtr> specsByCliString;

    std::string helpPrefix;
    std::vector<HelpGroup> helpSections;

    std::set<std::string> reservedNames;

    std::vector<std::pair<Flag, std::function<void()>>> terminalFlags;
};

template<>
inline std::string Parser::toString(const std::string& value) {
    return value;
}

}  // namespace mcga::cli
