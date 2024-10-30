idf_component_register(SRCS "config.c" "main.c" "telegram_bot.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES telegram_certificate.pem)
