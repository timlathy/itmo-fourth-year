package itmo.se.lab3.helpers

import org.junit.jupiter.api.extension.*
import org.openqa.selenium.WebDriver
import org.openqa.selenium.chrome.ChromeDriver
import org.openqa.selenium.firefox.FirefoxDriver
import java.lang.reflect.Method
import java.util.stream.Stream

@Target(AnnotationTarget.FUNCTION)
@Retention(AnnotationRetention.RUNTIME)
annotation class RunWithChrome

@Target(AnnotationTarget.FUNCTION)
@Retention(AnnotationRetention.RUNTIME)
annotation class RunWithFirefox

class WebDriverTemplateInvocationContextProvider : TestTemplateInvocationContextProvider {
    override fun supportsTestTemplate(ctx: ExtensionContext) = true

    override fun provideTestTemplateInvocationContexts(ctx: ExtensionContext): Stream<TestTemplateInvocationContext> =
        Stream.builder<TestTemplateInvocationContext>().apply {
            if (ctx.testMethod.map { it.getAnnotationsByType(RunWithChrome::class.java).isNotEmpty() }.orElse(false))
                add(invocationContext(ChromeDriver::class.java))
            if (ctx.testMethod.map { it.getAnnotationsByType(RunWithFirefox::class.java).isNotEmpty() }.orElse(false))
                add(invocationContext(FirefoxDriver::class.java))
        }.build()

    private fun invocationContext(driverClass: Class<out WebDriver>): TestTemplateInvocationContext {
        return object : TestTemplateInvocationContext {
            override fun getDisplayName(invocationIndex: Int) = driverClass.simpleName

            override fun getAdditionalExtensions(): List<Extension> {
                return listOf(object : ParameterResolver {
                    override fun supportsParameter(paramCtx: ParameterContext, extensionCtx: ExtensionContext) =
                        paramCtx.parameter.type == WebDriver::class.java

                    override fun resolveParameter(paramCtx: ParameterContext, extensionCtx: ExtensionContext) =
                        driverClass.getConstructor().newInstance()
                }, object : InvocationInterceptor {
                    override fun interceptTestTemplateMethod(
                        invocation: InvocationInterceptor.Invocation<Void>,
                        invocationContext: ReflectiveInvocationContext<Method>,
                        extensionContext: ExtensionContext?
                    ) {
                        invocation.proceed()
                        invocationContext.arguments.filterIsInstance<WebDriver>().first().quit()
                    }
                })
            }
        }
    }
}