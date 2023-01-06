import os
import textwrap
from unittest.mock import patch

from src.kconfig.kconfig import KConfig, main
from tests.utils import TestUtils


class TestKConfig:
    def test_create_json(self):
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        config NAME
            string "Just the name"
            default "John Smith"
        config STATUS
            string "Defines your status"
            default "ALIVE"
        """)
        iut = KConfig(feature_model_file)
        assert iut.get_json_values() == {'NAME': 'John Smith', 'STATUS': 'ALIVE'}
        
    def test_create_cmake(self):
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        config NAME
            string "Just the name"
            default "John Smith"
        config STATUS
            bool "Defines your status"
            default y
        """)
        iut = KConfig(feature_model_file)
        cmake_expected = 'set(NAME "John Smith")\nset(STATUS "True")'
        assert iut.get_cmake_content() == cmake_expected

    def test_load_user_config_file(self):
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        config NAME
            string "Just the name"
            default "No Name"
        """)
        user_config = out_dir.write_file('user.config', """
CONFIG_UNKNOWN="y"
CONFIG_NAME="John Smith" 
        """)

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'NAME': 'John Smith'}

    def test_load_user_config_file_with_malformed_content(self):
        """
        All lines in the user configuration file starting with whitespaces will be ignored
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        config NAME
            string "Just the name"
            default "No Name"
        """)
        user_config = out_dir.write_file('user.config', """
 CONFIG_NAME="John Smith" 
        """)

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'NAME': 'No Name'}

    def test_boolean_without_description(self):
        """
        A configuration without description can not be selected by the user
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        mainmenu "This is the main menu"
            menu "First menu"
                config FIRST_BOOL
                    bool
                config FIRST_NAME
                    string "You can select this"
                config SECOND_NAME
                    string
            endmenu
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_FIRST_BOOL=y
        CONFIG_FIRST_NAME="Dude"
        CONFIG_SECOND_NAME="King"
        """))

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'FIRST_NAME': 'Dude'}

    def test_boolean_with_description(self):
        """
        A configuration with description can be selected by the user
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        mainmenu "This is the main menu"
            menu "First menu"
                config FIRST_BOOL
                    bool "You can select this"
                config FIRST_NAME
                    string "You can select this also"
            endmenu
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_FIRST_BOOL=y
        CONFIG_FIRST_NAME="Dude"
        """))

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'FIRST_NAME': 'Dude', 'FIRST_BOOL': True}

    def test_define_boolean_choices(self):
        """
        Using a boolean choice will define a boolean for every value.
        Only the choices with a 'prompt' are selectable.
        There is a warning generated for choices without a 'prompt'.
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        choice APP_VERSION
            prompt "application version"
            default APP_VERSION_1
            help
                Currently there are several application version supported.
                Select the one that matches your needs.
    
            config APP_VERSION_1
                bool
                prompt "app v1"
            config APP_VERSION_2
                bool
                prompt "app v2"
            # This is not selectable because it has no prompt 
            config APP_VERSION_3
                bool
        endchoice
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_APP_VERSION="APP_VERSION_1"
        """))

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'APP_VERSION_1': True, 'APP_VERSION_2': False}

    def test_define_string_choices(self):
        """
        A choice can only be of type bool or tristate.
        One can use string but a warning will be issued.
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        choice APP_VERSION
            prompt "application version"
            default APP_VERSION_1
            help
                Currently there are several application version supported.
                Select the one that matches your needs.
    
            config APP_VERSION_1
                string
                prompt "app v1"
            config APP_VERSION_2
                string
                prompt "app v2"
        endchoice
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_APP_VERSION="APP_VERSION_1"
        CONFIG_APP_VERSION_1="VERSION_NEW"
        """))

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'APP_VERSION_1': 'VERSION_NEW', 'APP_VERSION_2': ''}

    def test_define_tristate_choices(self):
        """
        For KConfig, `bool` and `tristate` types are represented as JSON Booleans,
        the third `tristate` state is not supported.
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        choice APP_VERSION
            prompt "application version"
            default APP_VERSION_1
            help
                Currently there are several application version supported.
                Select the one that matches your needs.
    
            config APP_VERSION_1
                tristate
                prompt "app v1"
            config APP_VERSION_2
                tristate
                prompt "app v2"
        endchoice
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_APP_VERSION="APP_VERSION_1"
        """))

        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'APP_VERSION_1': True, 'APP_VERSION_2': False}

    def test_config_including_other_config(self):
        """
        Including other configuration files with 'source' works only as relative paths to the main file folder :(
        See how 'common.txt' must include 'new.txt' with its relative path to the main file.
        One can also use:
            * 'rsource' - for paths relative to the current file
            * 'osource' - for files that might not exist
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        menu "First menu"
            config FIRST_BOOL
                bool "You can select FIRST_BOOL"
            config FIRST_NAME
                string "You can select FIRST_NAME"
        endmenu
        source "common/common.txt"
        """)
        out_dir.write_file('common/common.txt', """
        config COMMON_BOOL
            bool "You can select COMMON_BOOL"
            default n
        source "new/new.txt"
        """)
        out_dir.write_file('new/new.txt', """
        config NEW_BOOL
            bool "You can select NEW_BOOL"
            default n
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_FIRST_BOOL=y
        CONFIG_FIRST_NAME="Dude"
        CONFIG_COMMON_BOOL=y
        CONFIG_NEW_BOOL=y
        """))
        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {
            'COMMON_BOOL': True,
            'FIRST_BOOL': True,
            'FIRST_NAME': 'Dude',
            'NEW_BOOL': True
        }

    def test_config_including_other_configs_based_on_env_vars(self):
        """
        One can refer to environment variables when including other files
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
        menu "First menu"
            config FIRST_BOOL
                bool "You can select FIRST_BOOL"
            config FIRST_NAME
                string "You can select FIRST_NAME"
        endmenu
        source "$(COMMON_PATH)/common.txt"
        """)
        out_dir.write_file('common/common.txt', """
        config COMMON_BOOL
            bool "You can select COMMON_BOOL"
            default n
        """)
        user_config = out_dir.write_file('user.config', textwrap.dedent("""
        CONFIG_FIRST_BOOL=y
        CONFIG_FIRST_NAME="Dude"
        CONFIG_COMMON_BOOL=y
        """))
        os.environ['COMMON_PATH'] = 'common'
        iut = KConfig(feature_model_file, user_config)
        assert iut.get_json_values() == {'COMMON_BOOL': True, 'FIRST_BOOL': True, 'FIRST_NAME': 'Dude'}

    def test_generate_header_file_running_main(self):
        """
        KConfigLib can generate the configuration as C-header file (like autoconf.h)
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
                menu "First menu"
                    config FIRST_BOOL
                        bool "You can select FIRST_BOOL"
                    config FIRST_NAME
                        string "You can select FIRST_NAME"
                endmenu
                """)

        user_config = out_dir.write_file('user.config', textwrap.dedent("""
                    CONFIG_FIRST_BOOL=y
                    CONFIG_FIRST_NAME="Dude"
                    """))
        header_file = out_dir.joinpath('gen/header.h')
        json_file = out_dir.joinpath('gen/features.json')
        cmake_file = out_dir.joinpath('gen/features.cmake')
        
        with patch('sys.argv', [
            'kconfig',
            '--kconfig_model_file', f"{feature_model_file}",
            '--kconfig_config_file', f"{user_config}",
            '--out_header_file', f"{header_file}",
            '--out_json_file', f"{json_file}",
            '--out_cmake_file', f"{cmake_file}",
        ]):
            main()
        assert json_file.exists()
        assert cmake_file.exists()
        assert header_file.exists()
        assert header_file.read_text() == """#ifndef autoconf\n#define autoconf\n\n#define CONFIG_FIRST_BOOL 1\n#define CONFIG_FIRST_NAME "Dude"\n\n#endif\n"""

    def test_header_file_written_when_changed(self):
        """
        KConfigLib can generate the configuration as C-header file (like autoconf.h)
        """
        out_dir = TestUtils.create_clean_test_dir('')
        feature_model_file = out_dir.write_file('kconfig.txt', """
                menu "First menu"
                    config FIRST_BOOL
                        bool "You can select FIRST_BOOL"
                    config FIRST_NAME
                        string "You can select FIRST_NAME"
                endmenu
                """)

        user_config = out_dir.write_file('user.config', textwrap.dedent("""
                    CONFIG_FIRST_BOOL=y
                    CONFIG_FIRST_NAME="Dude"
                    """))
        iut = KConfig(feature_model_file, user_config)
        header_file = out_dir.joinpath('gen/header.h')
        iut.generate_header(header_file)
        assert header_file.exists()
        assert header_file.read_text() == """#ifndef autoconf\n#define autoconf\n\n#define CONFIG_FIRST_BOOL 1\n#define CONFIG_FIRST_NAME "Dude"\n\n#endif\n"""
        timestamp = header_file.stat().st_ctime
        iut.generate_header(header_file)
        assert header_file.stat().st_ctime == timestamp, "the file shall not be written if content is not changed"
        header_file.write_text('')
        iut.generate_header(header_file)
        assert header_file.stat().st_ctime == timestamp, "overwrite the file if content changes"
